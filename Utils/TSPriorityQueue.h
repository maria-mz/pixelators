#ifndef TS_PRIORITY_QUEUE_H_
#define TS_PRIORITY_QUEUE_H_

#include <queue>
#include <vector>
#include <chrono>
#include <functional>


template <typename T>
class TSPriorityQueue
{
    public:
        using Comparator = std::function<bool(const T&, const T&)>;
        using MatchPredicate = std::function<bool(const T&)>;

        explicit TSPriorityQueue(Comparator comp) : m_pQueue(comp) {}

        void push(T item)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_pQueue.push(item);

            m_cv.notify_one();
        }

        T pop()
        {
            T item;

            std::unique_lock<std::mutex> lock(m_mtx);

            m_cv.wait(lock, [this]() { return !m_pQueue.empty(); });

            if (!m_pQueue.empty())
            {
                item = m_pQueue.top();
                m_pQueue.pop();
            }
            else
            {
                item = T{};
            }

            return item;
        }

        bool top(T &item)
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            if (!m_pQueue.empty())
            {
                item = m_pQueue.top();
                return true;
            }

            return false;
        }

        // TODO: Have a way to cancel this?
        bool pop_matching(T& item, MatchPredicate predicate, int timeout = -1)
        {
            auto wait_cond = [this, &predicate]()
            {
                return !m_pQueue.empty() && predicate(m_pQueue.top());
            };

            std::unique_lock<std::mutex> lock(m_mtx);

            if (timeout > 0)
            {
                m_cv.wait_for(lock, std::chrono::milliseconds(timeout), wait_cond);
            }
            else
            {
                m_cv.wait(lock, wait_cond);
            }

            if (!m_pQueue.empty() && predicate(m_pQueue.top()))
            {
                item = m_pQueue.top();
                m_pQueue.pop();
                return true;
            }

            return false;
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            return m_pQueue.empty();
        }

    private:
        std::priority_queue<T, std::vector<T>, Comparator> m_pQueue;
        std::mutex m_mtx;
        std::condition_variable m_cv;
};

#endif