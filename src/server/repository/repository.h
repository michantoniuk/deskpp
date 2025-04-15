#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <optional>
#include <functional>

// Generic repository interface for entity operations
template<typename T>
class Repository {
public:
    Repository() = default;

    virtual ~Repository() = default;

    // Core repository methods
    virtual std::vector<T> findAll() = 0;

    virtual std::optional<T> findById(int id) = 0;

    virtual T add(const T &entity) = 0;

    virtual bool update(const T &entity) = 0;

    virtual bool remove(int id) = 0;

    // Generic search method
    virtual std::vector<T> findBy(std::function<bool(const T &)> predicate) {
        auto all = findAll();
        std::vector<T> filtered;

        for (const auto &item: all) {
            if (predicate(item)) {
                filtered.push_back(item);
            }
        }

        return filtered;
    }
};

#endif // REPOSITORY_H
