#ifndef STRING_HPP
#define STRING_HPP

#include "vec.hpp"
#include "reflex.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

static std::string data_dir = "./";

template<typename O, typename T, std::size_t N>
O& operator << (O& o, const std::array<T,N>& v) {
    for (std::size_t i = 0; i < N; ++i) {
        if (i != 0) o << ", ";
        o << v[i];
    }
    
    return o;
}

template<typename T>
std::string strn(const T& t) {
    std::ostringstream ss;
    ss << reflex::wrap(t);
    return ss.str();
}

template<typename T>
std::string strn(const T& t, std::size_t n, char fill = '0') {
    if (n <= 1) {
        return strn(t);
    }
    if (t == 0) {
        return std::string(n-1, fill)+'0';
    } else {
        std::ostringstream ss;
        ss << t;
        std::size_t nz = n-1 - floor(log10(t));
        if (nz > 0 && nz < 6) {
            return std::string(nz, fill) + ss.str();
        } else {
            return ss.str();    
        }
    }
}

template<std::size_t Dim, typename Type>
vec_t<Dim,std::string> strna(const vec_t<Dim,Type>& v) {
    vec_t<Dim,std::string> s = strarr(v.dims);
    for (std::size_t i = 0; i < v.data.size(); ++i) {
        s.data[i] = strn(dref(v.data[i]));
    }
    
    return s;
}

template<std::size_t Dim, typename Type>
vec_t<Dim,std::string> strna(const vec_t<Dim,Type>& v, std::size_t n, char fill = '0') {
    vec_t<Dim,std::string> s = strarr(v.dims);
    for (std::size_t i = 0; i < v.data.size(); ++i) {
        s.data[i] = strn(dref(v.data[i]), n, fill);
    }
    
    return s;
}

template<typename T>
bool from_string(const std::string& s, T& t) {
    std::istringstream ss(s);
    return (ss >> t);
}

template<typename T>
T nstr(const std::string& s) {
    std::istringstream ss(s);
    T t;
    assert(ss >> t);
    return t;
}

std::string trim(const std::string& ts, const std::string& chars = " \t") {
    std::string s = ts;
    std::size_t spos = s.find_first_of(chars);
    if (spos == 0) {
        std::size_t epos = s.find_first_not_of(chars);
        if (epos == s.npos) return "";
        s = s.substr(epos);
    }
    
    spos = s.find_last_of(chars);
    if (spos == s.size()-1) {
        std::size_t epos = s.find_last_not_of(chars);
        s = s.erase(epos+1, s.size() - epos+1);
    }
    
    return s;
}

std::string toupper(const std::string& ts) {
    std::string s = ts;
    for (auto& c : s) {
        c = ::toupper(c);
    }
    return s;
}

std::string tolower(const std::string& ts) {
    std::string s = ts;
    for (auto& c : s) {
        c = ::tolower(c);
    }
    return s;
}

std::string erase_begin(const std::string& ts, uint_t n) {
    if (n >= ts.size()) {
        return "";
    }
    
    std::string s = ts;
    return s.erase(0, n);
}

std::string erase_end(const std::string& ts, uint_t n) {
    if (n >= ts.size()) {
        return "";
    }
    
    std::string s = ts;
    return s.erase(ts.size()-n, n);
}

std::string replace(const std::string& ts, const std::string& pattern, const std::string& rep) {
    std::string s = ts;
    auto p = s.find(pattern);
    while (p != s.npos) {
        s.replace(p, pattern.size(), rep);
        p = s.find(pattern, p+1);
    }
    
    return s;
}

bool empty(const std::string& ts) {
    return ts.empty();
}

uint_t distance(const std::string& t, const std::string& u) {
    uint_t n = std::min(t.size(), u.size());
    uint_t d = abs(t.size() - u.size());
    for (uint_t i = 0; i < n; ++i) {
        if (t[i] != u[i]) ++d;
    }

    return d;
}

int_t find(const std::string& ts, const std::string& pattern) {
    auto p = ts.find(pattern);
    if (p != ts.npos) {
        return p;
    } else {
        return -1;
    }
}

#define VECTORIZE(name) \
    template<std::size_t Dim, typename Type, typename ... Args, \
        typename enable = typename std::enable_if< \
            std::is_same<typename std::remove_pointer<Type>::type, std::string>::value>::type> \
    auto name(const vec_t<Dim,Type>& v, const Args& ... args) -> \
        vec_t<Dim,decltype(name(v[0], args...))> { \
        using ntype = decltype(name(v[0], args...)); \
        vec_t<Dim,ntype> r = arr<ntype>(v.dims); \
        for (std::size_t i = 0; i < v.data.size(); ++i) { \
            r.data[i] = name(dref(v.data[i]), args...); \
        } \
        return r; \
    }

VECTORIZE(trim)
VECTORIZE(toupper)
VECTORIZE(tolower)
VECTORIZE(erase_begin)
VECTORIZE(erase_end)
VECTORIZE(empty)
VECTORIZE(distance)
VECTORIZE(find)
VECTORIZE(replace)

#undef VECTORIZE

template<typename T>
vec1s split(const std::string& ts, const T& pattern) {
    vec1s ret;
    std::size_t p = 0, op = 0;
    while ((p = ts.find(pattern, op)) != ts.npos) {
        ret.data.push_back(ts.substr(op, p - op));
        op = p+1;
    }

    ret.data.push_back(ts.substr(op));
    ret.dims[0] = ret.data.size();

    return ret;
}

#endif
