#include <iostream>
#include <string>
#include <chrono>
#include <cstdint>
#include <ctime>

class Snowflake {
public:
    Snowflake(uint16_t datacenterId, uint16_t workerId)
        : datacenterId_(datacenterId), workerId_(workerId), sequence_(0), lastTimestamp_(0) {
        if (datacenterId_ > maxDatacenterId_ || workerId_ > maxWorkerId_) {
            throw std::invalid_argument("Invalid datacenter or worker ID");
        }
    }

    uint64_t generateUniqueId() {
        uint64_t timestamp = currentTimestamp();

        if (timestamp < lastTimestamp_) {
            throw std::runtime_error("Clock moved backwards");
        }
		
		//时间戳原理:获取当前时间戳如果等于上次时间戳(同一毫秒内),咋序列号加一。否则序列号设置为0，从0开始
        if (timestamp == lastTimestamp_) {
            sequence_ = (sequence_ + 1) & sequenceMask_;
            if (sequence_ == 0) {
                timestamp = waitNextMillis(lastTimestamp_);
            }
        } else {
            sequence_ = 0;
        }

        lastTimestamp_ = timestamp;

        return ((timestamp - twepoch_) << timestampLeftShift_) |
               ((datacenterId_ & maxDatacenterId_) << datacenterIdShift_) |
               ((workerId_ & maxWorkerId_) << workerIdShift_) |
               (sequence_ & sequenceMask_);
    }

private:
    uint64_t currentTimestamp() const {
        return std::time(nullptr) * 1000;
    }

    uint64_t waitNextMillis(uint64_t lastTimestamp) {
        uint64_t timestamp = currentTimestamp();
        while (timestamp <= lastTimestamp) {
            timestamp = currentTimestamp();
        }
        return timestamp;
    }

    const uint64_t twepoch_ = 1609459200000ULL;  // 2021-01-01 00:00:00 UTC
    const uint64_t unixEpoch_ = 0ULL;  // Unix时间戳起点：1970-01-01 00:00:00 UTC
    const uint64_t workerIdBits_ = 5;
    const uint64_t datacenterIdBits_ = 5;
    const uint64_t maxWorkerId_ = (1ULL << workerIdBits_) - 1;
    const uint64_t maxDatacenterId_ = (1ULL << datacenterIdBits_) - 1;
    const uint64_t sequenceBits_ = 12;
    const uint64_t workerIdShift_ = sequenceBits_;
    const uint64_t datacenterIdShift_ = sequenceBits_ + workerIdBits_;
    const uint64_t timestampLeftShift_ = sequenceBits_ + workerIdBits_ + datacenterIdBits_;
    const uint64_t sequenceMask_ = (1ULL << sequenceBits_) - 1;

    uint16_t datacenterId_;
    uint16_t workerId_;
    uint64_t sequence_;
    uint64_t lastTimestamp_;
};

static inline uint16_t datacenter_id = 10;
static inline uint16_t worker_id = 20;
static inline Snowflake snowflake = Snowflake(datacenter_id, worker_id);


class grand_parent {
private:
    int value = 0;
    std::chrono::_V2::system_clock::time_point now;
    std::string id;
public:
    grand_parent() {
        id = std::to_string(snowflake.generateUniqueId());
        std::cout << "grand parent of: " << id << " have been create. value is: " << ++value << std::endl;
    }
    ~grand_parent() {
        std::cout << "grand parent of: " << id << " have been destroy" << std::endl;
    }
};

class parent : public grand_parent {
private:

    std::chrono::_V2::system_clock::time_point now;
    std::string id;
public:
    parent() {
        id = std::to_string(snowflake.generateUniqueId());
        std::cout << "parent of: " << id << " have been create" << std::endl;
    }
    ~parent() {
        std::cout << "parent of: " << id << " have been destroy" << std::endl;
    }
};



class uncle : grand_parent {
private:
    std::string id;
public:
    uncle() {
        id = std::to_string(snowflake.generateUniqueId());
        std::cout << "uncle of: " << id << " have been create"<< std::endl;
        
    }
    ~uncle() {
        std::cout << "uncle of: " << id << " have been destroy" << std::endl;
    }
};

class obj : public parent, uncle {
private:
    std::string id;
public:
    obj() {
        id = std::to_string(snowflake.generateUniqueId());
        std::cout << "obj of: " << id << " have been create" << std::endl;
    }
    ~obj() {
        std::cout << "obj of: " << id << " have been destroy" << std::endl;
    }
};


void function() {
    obj a = *(new obj());
}

int main(int argc, char* argv[]) {
    std::cout << "before calling function" << std::endl;
    function();
    std::cout << "after calling function" << std::endl;
}