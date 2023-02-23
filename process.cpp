#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstring>

using namespace std;

#define SHM_SIZE 1024

// 共享内存结构体
struct SharedMemory {
    int buffer[6];  // 循环队列
    int front;      // 队首指针
    int rear;       // 队尾指针
    int count;      // 队列元素个数
    pthread_mutex_t mutex;  // 互斥锁
};

// 全局变量
int shm_fd;                 // 共享内存文件描述符
SharedMemory* shm_ptr;      // 共享内存指针

// 写者线程函数
void* writer(void* arg) {
    int data[] = {8, 9, 4, 1, 2, 6};
    int index = 0;
    while (index < 6) {
        pthread_mutex_lock(&shm_ptr->mutex);  // 加锁
        // 循环队列已满，等待读者线程读取元素
        while (shm_ptr->count == 6) {
            pthread_mutex_unlock(&shm_ptr->mutex);    // 先解锁
            usleep(rand() % 100);  // 随机等待一段时间
            pthread_mutex_lock(&shm_ptr->mutex);      // 再加锁
        }
        // 写入元素到队尾
        shm_ptr->buffer[shm_ptr->rear] = data[index];
        shm_ptr->rear = (shm_ptr->rear + 1) % 6;
        shm_ptr->count++;
        cout << "Writer: wrote " << data[index] << " to queue" << endl;
        index++;
        pthread_mutex_unlock(&shm_ptr->mutex);    // 解锁
        usleep(rand() % 100);  // 随机等待一段时间
    }
    return NULL;
}

// 读者线程函数
void* reader(void* arg) {
    int id = *(int*) arg;
    while (true) {
        pthread_mutex_lock(&shm_ptr->mutex);  // 加锁
        // 循环队列为空，等待写者线程写入元素
        while (shm_ptr->count == 0) {
            pthread_mutex_unlock(&shm_ptr->mutex);    // 先解锁
            usleep(rand() % 100);  // 随机等待一段时间
            pthread_mutex_lock(&shm_ptr->mutex);      // 再加锁
        }
        // 读取队首元素
        int data = shm_ptr->buffer[shm_ptr->front];
        shm_ptr->front = (shm_ptr->front + 1) % 6;
        shm_ptr->count--;
        cout << "Reader " << id << ": read " << data << " from queue" << endl;
        pthread_mutex_unlock(&shm_ptr->mutex);    // 解锁
        usleep(rand() % 100);  // 随机等待一段时间
    }
    return NULL;
}

int main() {
    // 创建共享内存
    shm_fd = shm_open("/myshm", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd == -1)
{
    cerr << "Failed to create shared memory" << endl;
    return 1;
}
if (ftruncate(shm_fd, SHM_SIZE) == -1) {
    cerr << "Failed to resize shared memory" << endl;
    return 1;
}
shm_ptr = (SharedMemory*) mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
if (shm_ptr == MAP_FAILED) {
    cerr << "Failed to map shared memory" << endl;
    return 1;
}

// 初始化共享内存
memset(shm_ptr, 0, SHM_SIZE);
pthread_mutex_init(&shm_ptr->mutex, NULL);

// 创建写者线程
pthread_t writer_thread;
pthread_create(&writer_thread, NULL, writer, NULL);

// 创建读者线程
pthread_t reader_threads[3];
int reader_ids[3] = {1, 2, 3};
for (int i = 0; i < 3; i++) {
    pthread_create(&reader_threads[i], NULL, reader, &reader_ids[i]);
}

// 等待写者线程和读者线程结束
pthread_join(writer_thread, NULL);
for (int i = 0; i < 3; i++) {
    pthread_join(reader_threads[i], NULL);
}

// 释放共享内存和互斥锁资源
munmap(shm_ptr, SHM_SIZE);
shm_unlink("/myshm");
pthread_mutex_destroy(&shm_ptr->mutex);

return 0;


// g++ -std=c++11 -pthread test5.cpp -lrt -o test5.exe  编译命令




}
