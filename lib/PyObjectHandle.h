#ifndef PYOBJECTHANDLE_H
#define PYOBJECTHANDLE_H

#include <Python.h>

// Taken from https://github.com/fedorajzf/fblualib ... more or less
class PyObjectHandle
{
public:
    PyObjectHandle() : obj_(nullptr) { }
    /* implicit */ PyObjectHandle(std::nullptr_t) noexcept : obj_(nullptr) { }

    // Construct, without incrementing obj's reference count
    explicit PyObjectHandle(PyObject* obj) noexcept : obj_(obj) { }

    enum IncRef { INCREF };
    // Construct, incrementing obj's reference count
    PyObjectHandle(IncRef, PyObject* obj) noexcept : obj_(obj) {
        Py_XINCREF(obj_);
    }

    PyObjectHandle(const PyObjectHandle& other) noexcept : obj_(other.obj_) {
        Py_XINCREF(obj_);
    }

    PyObjectHandle(PyObjectHandle&& other) noexcept : obj_(other.obj_) {
        other.obj_ = nullptr;
    }

    ~PyObjectHandle() noexcept {
        reset();
    }

    PyObjectHandle& operator=(const PyObjectHandle& other) noexcept {
        if (this != &other) {
            // This is safe even if obj_ == other.obj_, that would imply that the
            // refcount is at least 2, so the Py_XDECREF leaves it at least 1.
            Py_XDECREF(obj_);
            obj_ = other.obj_;
            Py_XINCREF(obj_);
        }
        return *this;
    }

    PyObjectHandle& operator=(PyObjectHandle&& other) noexcept {
        if (this != &other) {
            if (obj_) {
                // This is safe even if obj_ == other.obj_, that would imply that the
                // refcount is at least 2, so the Py_XDECREF leaves it at least 1.
                Py_DECREF(obj_);
            }
            obj_ = other.obj_;
            other.obj_ = nullptr;
        }
        return *this;
    }

    // Return the PyObject* contained in the handle
    PyObject* get() const noexcept { return obj_; }

    // Return the PyObject* contained in the handle and release all claim
    // to it (that is, we won't Py_DECREF it in the destructor); use it to pass
    // as argument to the (few) Python API functions that steal references.
    PyObject* release() noexcept {
        PyObject* o = obj_;
        obj_ = nullptr;
        return o;
    }

    // Reset to point to another object; do not increment obj's reference count
    void reset(PyObject* obj = nullptr) noexcept {
        if (obj_) {
            Py_DECREF(obj_);
        }
        obj_ = obj;
    }

    // Reset to point to another object; increment obj's reference count
    void reset(IncRef, PyObject* obj) noexcept {
        Py_XINCREF(obj);
        Py_XDECREF(obj_);
        obj_ = obj;
    }

    PyObject* operator->() const noexcept { return get(); }
    PyObject& operator*() const noexcept { assert(get()); return *get(); }

    explicit operator bool() const noexcept { return obj_; }

private:
    PyObject* obj_;
};


#endif // PYOBJECTHANDLE_H
