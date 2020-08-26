#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <chrono>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <windows.h>
#include <conio.h>

using namespace std::chrono_literals;

using Task = std::function<void(void)>;

class TaskPool {
  public:
    void addTask(Task task) {
        {
            std::lock_guard lock(m);
            tasks.push(std::move(task));
        }
        v.notify_one();
    }
    
    Task getTask() {
        Task task;
        {
            std::unique_lock lock(m);
            if(tasks.empty()) v.wait(lock, [&] { return !tasks.empty() || !work; });
            if(work) {
                task = tasks.front();
                tasks.pop();
            }
        }
        return task ? task : Task();
    }
    
    void stopWait() {
        work = false;
        v.notify_all();
    }
  
  
  private:
    std::queue<Task> tasks;
    std::mutex m;
    std::condition_variable v;
    bool work = true;
};

struct Thread {
    Thread(std::thread&& t, size_t serial) {
        thread.swap(t);
        serial_number = serial;
    }
    std::thread thread;
    size_t serial_number;
    std::atomic<bool> done = false;
};

class ThreadPool {
  public:
    ThreadPool(TaskPool& task_pool, size_t threads_count)
    : task_pool(task_pool)
    {
        for(size_t i = 0; i != threads_count; ++i) {
            std::unique_ptr<Thread> u = std::make_unique<Thread>(
                std::thread(&ThreadPool::worker, this), i
            );
            threads[u->thread.get_id()] = std::move(u);
        }
    }
    ~ThreadPool() {
        work = false;
        task_pool.stopWait();
        for(auto& [id, t]: threads) {
            t->thread.join();
        }
    }
    
    void worker() {
        Thread& thread = *threads[std::this_thread::get_id()];
        while(work) {
            thread.done = true;
            Task task = task_pool.getTask();
            if(task) {
                thread.done = false;
                task();
                thread.done = true;
            }
        }
    }
    
    size_t getId(std::thread::id id) {
        return threads[id]->serial_number;
        
    }
    
    bool done() const {
        for(auto const& [id, t]: threads) {
            if(!t->done) {
                return false;
            }
        }
        return true;
    }
  
  private:
    TaskPool& task_pool;
    std::map<std::thread::id, std::unique_ptr<Thread>> threads;
    bool work = true;
};

void setCursor(int row, int col, int background = 0xF, int foreground = 0x0) {
    static HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    static struct S {
        S() {
            MoveWindow(GetConsoleWindow(), 100, 100, 1256, 600, TRUE);
        }
    } set_size;
    SetConsoleCursorPosition(console, {short(col), short(row)});
    SetConsoleTextAttribute(console, byte(background << 4u | foreground));
}

int main() {
    TaskPool task_pool;
    ThreadPool thread_pool(task_pool, std::thread::hardware_concurrency() - 1);
    std::mutex cout_mutex;
    
    int tasks;
    std::map<int, int> count_invokes;
    while(isdigit(tasks = _getch())) {
        if(thread_pool.done()) system("color 0f & cls");
        for(int cur_task = '0'; cur_task != tasks; ++cur_task) {
            task_pool.addTask([&cout_mutex, &thread_pool, &count_invokes] {
                int id = int(thread_pool.getId(std::this_thread::get_id()));
                int& runs = count_invokes[id];
                for(int j = 0; j != 20; ++j) {
                    std::lock_guard lock(cout_mutex);
                    setCursor(id*3, j*5, abs(0xF - runs*4), runs/2 % 0x10);
                    printf("%c%i", 'A'+id, j);
                    std::this_thread::sleep_for(20ms);
                }
                ++runs;
            });
        }
    }
}
