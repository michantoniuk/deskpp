#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <optional>

template<typename T>
class Repository {
public:
    virtual ~Repository() = default;

    virtual std::vector<T> findAll() = 0;

    virtual std::optional<T> findById(int id) = 0;

    virtual T add(const T &entity) = 0;

    virtual bool update(const T &entity) = 0;

    virtual bool remove(int id) = 0;
};

#endif // REPOSITORY_H
