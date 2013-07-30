#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "vec.hpp"
#include "math.hpp"

template<typename Type>
typename vec_t<2,Type>::effective_type enlarge(const vec_t<2,Type>& v, int_t pix, const typename vec_t<2,Type>::rtype& def = 0.0) {
    if (pix >= 0) {
        typename vec_t<2,Type>::effective_type r = dblarr(v.dims[0]+2*pix, v.dims[1]+2*pix) + def;
        
        for (int_t y : rgen(v.dims[0]))
        for (int_t x : rgen(v.dims[1])) {
            r(x+pix,y+pix) = v(x,y); 
        }
        
        return r;
    } else {
        assert(uint_t(-2*pix) < v.dims[0] && uint_t(-2*pix) < v.dims[1]);
        
        typename vec_t<2,Type>::effective_type r = dblarr(v.dims[0]+2*pix, v.dims[1]+2*pix);
        
        for (int_t y : rgen(r.dims[0]))
        for (int_t x : rgen(r.dims[1])) {
            r(x,y) = v(x-pix,y-pix); 
        }
        
        return r;
    }
}

// Get a sub region 'reg' inside an image. reg = {x0, y0, x1, y1}.
template<typename TypeV, typename TypeR = int_t>
void subregion(const vec_t<2,TypeV>& v, const vec_t<1,TypeR>& reg, vec1i& rr, vec1i& rs) {
    assert(n_elements(reg) == 4);
    int_t nvx = v.dims[1], nvy = v.dims[0];
    int_t nx = reg(2)-reg(0)+1, ny = reg(3)-reg(1)+1;
    
    vec1i vreg = reg;
    vec1i sreg = {0,0,nx-1,ny-1};
    
    if (reg(0) >= nvx || reg(0) > reg(2) || reg(1) >= nvy || reg(1) > reg(3)) {
        rr = rs = intarr(0);
        return;
    }
    
    if (reg(0) < 0) {
        vreg(0) = 0;
        sreg(0) += 0 - reg(0);
    }
    if (reg(2) >= nvx) {
        vreg(2) = nvx-1;
        sreg(2) -= reg(2) - (nvx-1);
    }
    
    if (reg(1) < 0) {
        vreg(1) = 0;
        sreg(1) += 0 - reg(1);
    }
    if (reg(3) >= nvx) {
        vreg(3) = nvy-1;
        sreg(3) -= reg(3) - (nvy-1);
    }
    
    vec1i vx = rgen(vreg(0), vreg(2));
    vec1i vy = rgen(vreg(1), vreg(3));
    vec1i sx = rgen(sreg(0), sreg(2));
    vec1i sy = rgen(sreg(1), sreg(3));
    
    rr = flatten(indgen(nvy,nvy)(vy,vx));
    rs = flatten(indgen(ny,nx)(sy,sx));
}

template<typename TypeV, typename TypeR = int_t>
typename vec_t<2,TypeV>::effective_type subregion(const vec_t<2,TypeV>& v, const vec_t<1,TypeR>& reg, const typename vec_t<2,TypeV>::rtype& def = 0.0) {
    vec1i rr, rs;
    subregion(v, reg, rr, rs);
    
    int_t nx = reg(2)-reg(0)+1, ny = reg(3)-reg(1)+1;
    vec_t<2,rtype_t<TypeV>> sub = arr<rtype_t<TypeV>>(ny,nx) + def;
    
    sub[rs] = v[rr];
    
    return sub;
}

//typename vec_t<2,TypeV>::effective_type subregion(const vec_t<2,TypeV>& v, const vec_t<1,TypeR>& reg, const typename vec_t<2,TypeV>::rtype& def = 0.0) {
//    assert(n_elements(reg) == 4);
//    int_t nvx = v.dims[1], nvy = v.dims[0];
//    int_t nx = reg(2)-reg(0)+1, ny = reg(3)-reg(1)+1;
//    
//    vec1i vreg = reg;
//    vec1i sreg = {0,0,nx-1,ny-1};
//    
//    vec2d sub = arr<typename vec_t<2,TypeV>::rtype>(ny,nx) + def;
//    
//    if (reg(0) >= nvx || reg(0) > reg(2) || reg(1) >= nvy || reg(1) > reg(3)) {
//        return sub;
//    }
//    
//    if (reg(0) < 0) {
//        vreg(0) = 0;
//        sreg(0) += 0 - reg(0);
//    }
//    if (reg(2) >= nvx) {
//        vreg(2) = nvx-1;
//        sreg(2) -= reg(2) - (nvx-1);
//    }
//    
//    if (reg(1) < 0) {
//        vreg(1) = 0;
//        sreg(1) += 0 - reg(1);
//    }
//    if (reg(3) >= nvx) {
//        vreg(3) = nvy-1;
//        sreg(3) -= reg(3) - (nvy-1);
//    }
//    
//    vec1i vx = rgen(vreg(0), vreg(2));
//    vec1i vy = rgen(vreg(1), vreg(3));
//    vec1i sx = rgen(sreg(0), sreg(2));
//    vec1i sy = rgen(sreg(1), sreg(3));
//    
//    sub(sy,sx) = v(vy,vx);
//    
//    return sub;
//}

vec2d circular_mask(vec1u dim, const vec1d& center, double radius) {
    if (n_elements(dim) == 0) {
        dim = {uint_t(radius), uint_t(radius)};
    } else if (n_elements(dim) == 1) {
        dim = {dim[0], dim[0]}; 
    }
    
    assert(n_elements(center) == 2);
    
    vec3d m = dblarr(dim(0), dim(1), 4);

    vec2d px = replicate(dindgen(dim(1)), dim(0));
    vec2d py = transpose(replicate(dindgen(dim(0)), dim(1)));
    
    m(_,_,0) = pow(px-0.5 - center(1),2) + pow(py-0.5 - center(0),2) <= radius*radius;
    m(_,_,1) = pow(px+0.5 - center(1),2) + pow(py-0.5 - center(0),2) <= radius*radius;
    m(_,_,2) = pow(px+0.5 - center(1),2) + pow(py+0.5 - center(0),2) <= radius*radius;
    m(_,_,3) = pow(px-0.5 - center(1),2) + pow(py+0.5 - center(0),2) <= radius*radius;
    
    return mean(m,2);
}

#endif