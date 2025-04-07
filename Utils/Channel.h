#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <mutex>
#include <deque>
#include <vector>
#include <chrono>
#include <condition_variable>

template <typename T>
class Channel
{
    public:
        void setOrderFunc(std::function<int(T&)> getOrder)
        {
            m_getOrder = std::move(getOrder);
        }

        bool write(T item)
        {
            std::lock_guard<std::mutex> lock(m_mtx);

            if (m_getOrder)
            {
                auto it = m_deque.begin();
                for (; it != m_deque.end(); ++it)
                {
                    if (m_getOrder(*it) >= m_getOrder(item))
                    {
                        break;
                    }
                }

                m_deque.insert(it, item);
            }
            else
            {
                m_deque.push_back(item);
            }

            m_cv.notify_all();

            return true;
        }

        T read_one(std::chrono::milliseconds timeout = std::chrono::milliseconds(2000))
        {
            std::unique_lock<std::mutex> lock(m_mtx);

            m_cv.wait_for(lock, timeout, [this]() { return !m_deque.empty(); });

            T item = m_deque.front();
            m_deque.pop_front();

            return item;
        }

        bool front(T &item)
        {
            bool ok = false;

            std::unique_lock<std::mutex> lock(m_mtx);
            if (!m_deque.empty())
            {
                item = m_deque.front();
                ok = true;
            }

            return ok;
        }

        bool read_one_with_order(
            T &item,
            int order,
            std::chrono::milliseconds timeout = std::chrono::milliseconds(2000)
        )
        {
            bool ok = false;

            std::unique_lock<std::mutex> lock(m_mtx);
            m_cv.wait_for(lock, timeout, [this, order]() {
                return !m_deque.empty() && m_getOrder(m_deque.front()) == order;
            });

            if (!m_deque.empty() && m_getOrder(m_deque.front()) == order)
            {
                item = m_deque.front();
                m_deque.pop_front();
                ok = true;
            }

            return ok;
        }

        void flush(std::vector<T> &buffer)
        {
            std::lock_guard<std::mutex> lock(m_mtx);

            while (!m_deque.empty())
            {
                buffer.push_back(m_deque.front());
                m_deque.pop_front();
            }
        }

        void clear()
        {
            std::lock_guard<std::mutex> lock(m_mtx);

            while (!m_deque.empty())
            {
                m_deque.pop_front();
            }
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            return m_deque.empty();
        }

    private:
        std::deque<T> m_deque;
        std::mutex m_mtx;
        std::condition_variable m_cv;

        std::function<int(T&)> m_getOrder = nullptr;
};

#endif