#pragma once

#include <stdexcept>

namespace Web {

/**
 * @brief The HandlerBase class Base for pointer handlers
 */
template <typename PointerT>
class HandlerBase
{
    // Extra metaclasses
    template <typename T, typename = void>
    struct is_compareAvailable : std::false_type {};

    template <typename T>
    struct is_compareAvailable<T, std::void_t<decltype(std::declval<T>().operator<(T{}))>> : std::true_type {};

public:
    HandlerBase();
    explicit HandlerBase(PointerT* pTarget);
    ~HandlerBase();

    // Pointer access operators
    PointerT* operator->() noexcept(false);
    const PointerT* operator->() const;

    // Copy of STL smart pointers logic
    PointerT* get();
    const PointerT* get() const;
    operator bool() const;

    bool isValid() const noexcept;

    // For std::set and others using
    bool operator<(const HandlerBase& _ohdl) const;

private:
    PointerT* m_pTarget {nullptr};

protected:
    // Add space for reimplement
    virtual void setPointer(PointerT* pTarget);
};


// =============================== IMPLEMENTATION ===================================== //

template<typename PointerT>
inline HandlerBase<PointerT>::HandlerBase() = default;

template<typename PointerT>
inline HandlerBase<PointerT>::HandlerBase(PointerT *pTarget) { setPointer(pTarget); }

template<typename PointerT>
inline HandlerBase<PointerT>::~HandlerBase() = default;

template<typename PointerT>
inline PointerT *HandlerBase<PointerT>::operator->() noexcept(false) {
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return m_pTarget;
}

template<typename PointerT>
inline const PointerT *HandlerBase<PointerT>::operator->() const noexcept(false) {
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return m_pTarget;
}

template<typename PointerT>
inline PointerT *HandlerBase<PointerT>::get() { return m_pTarget; }

template<typename PointerT>
inline const PointerT *HandlerBase<PointerT>::get() const { return m_pTarget; }

template<typename PointerT>
inline HandlerBase<PointerT>::operator bool() const { return isValid(); }

template<typename PointerT>
inline bool HandlerBase<PointerT>::isValid() const noexcept { return (nullptr != m_pTarget); }

template<typename PointerT>
inline bool HandlerBase<PointerT>::operator<(const HandlerBase &_ohdl) const {
    if (!isValid()) {
        return _ohdl.isValid();
    }
    if constexpr (is_compareAvailable<PointerT>::value) {
        return *m_pTarget < *_ohdl.m_pTarget;
    } else {
        return m_pTarget < _ohdl.m_pTarget;
    }
}

template<typename PointerT>
inline void HandlerBase<PointerT>::setPointer(PointerT *pTarget) {
    m_pTarget = pTarget;
}

} // namespace Web
