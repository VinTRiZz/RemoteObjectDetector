#pragma once

#include <memory>
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
    HandlerBase() = default;
    explicit HandlerBase(PointerT* pTarget);
    ~HandlerBase() = default;

    // Analog of std::static_pointer_cast
    template <typename DerivedT>
    std::enable_if_t<std::is_base_of_v<PointerT, DerivedT> || std::is_base_of_v<DerivedT, PointerT>,
                     DerivedT*>
    cast();
    template <typename DerivedT>
    std::enable_if_t<std::is_base_of_v<PointerT, DerivedT> || std::is_base_of_v<DerivedT, PointerT>,
                     const DerivedT*>
    cast() const;

    // Analog of std::dynamic_pointer_cast
    template <typename DerivedT>
    std::enable_if_t<std::is_base_of_v<PointerT, DerivedT> || std::is_base_of_v<DerivedT, PointerT>,
                     DerivedT*>
    cast_dynamic();
    template <typename DerivedT>
    std::enable_if_t<std::is_base_of_v<PointerT, DerivedT> || std::is_base_of_v<DerivedT, PointerT>,
                     const DerivedT*>
    cast_dynamic() const;

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
    std::shared_ptr<bool> m_isValid { std::make_shared<bool>(false) };
    PointerT* m_pTarget {nullptr}; // Pointer for synchronization

protected:
    // Add space for reimplement
    virtual void setPointer(PointerT* pTarget);
    void invalidate();
};

// =============================== IMPLEMENTATION ===================================== //

template<typename PointerT>
inline HandlerBase<PointerT>::HandlerBase(PointerT *pTarget) { setPointer(pTarget); }

template <typename PointerT>
template <typename DerivedT>
std::enable_if_t<std::is_base_of_v<PointerT, DerivedT> || std::is_base_of_v<DerivedT, PointerT>, DerivedT*>
HandlerBase<PointerT>::cast()
{
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return static_cast<DerivedT*>(m_pTarget);
}

template <typename PointerT>
template <typename DerivedT>
std::enable_if_t<std::is_base_of_v<PointerT, DerivedT> || std::is_base_of_v<DerivedT, PointerT>, const DerivedT*>
HandlerBase<PointerT>::cast() const
{
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return static_cast<const DerivedT*>(m_pTarget);
}

template <typename PointerT>
template <typename DerivedT>
std::enable_if_t<std::is_base_of_v<PointerT, DerivedT> || std::is_base_of_v<DerivedT, PointerT>, DerivedT*>
HandlerBase<PointerT>::cast_dynamic()
{
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return dynamic_cast<DerivedT*>(m_pTarget);
}

template <typename PointerT>
template <typename DerivedT>
std::enable_if_t<std::is_base_of_v<PointerT, DerivedT> || std::is_base_of_v<DerivedT, PointerT>, const DerivedT*>
HandlerBase<PointerT>::cast_dynamic() const
{
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return dynamic_cast<const DerivedT*>(m_pTarget);
}

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
inline bool HandlerBase<PointerT>::isValid() const noexcept { return *m_isValid; }

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
    // TODO: Set pointer is not thread-safe... Think about it
    *m_isValid = false;
    m_pTarget = pTarget;
    *m_isValid = (m_pTarget != nullptr);
}

template<typename PointerT>
inline void HandlerBase<PointerT>::invalidate()
{
    *m_isValid = false;
}

} // namespace Web
