#include "../allheader.h"

template<class T>
class BlockingQueue {
public:
    // 将元素添加到队列
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        cond_var_.notify_one();
    }

    // 从队列中获取元素，如果队列为空则阻塞
    // 如果超时时间为0，则表示无限等待
    // 如果超时时间大于0，则表示等待指定的时间
    bool pop(T& value, const unsigned int timeout_ms = 0) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (timeout_ms == 0) {
            // 等待队列非空
            while (queue_.empty()) {
                cond_var_.wait(lock);
            }
        } else {
            // 等待指定时间或队列非空
            auto timeout = std::chrono::milliseconds(timeout_ms);
            while (queue_.empty()) {
                if (cond_var_.wait_for(lock, timeout) == std::cv_status::timeout) {
                    return false;
                }
            }
        }
        value = queue_.front();
        queue_.pop();
        return true;
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
};