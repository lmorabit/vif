#include <phypp.hpp>

#define check(t, s) { \
    std::string st = trim(strn(t), "[]"); \
    if (st == s) print("checked: "+st); \
    else         print("failed: "+std::string(#t)+" = "+st+" != "+s); \
    assert(st == s); \
}

int main(int argc, char* argv[]) {
    file::mkdir("out");

    {
        // Type checking on array indexation
        static_assert(std::is_same<make_vrtype<3,float,int_t,int_t,int_t>::type, float&>::value, "wrong type of indexed array");
        static_assert(std::is_same<make_vrtype<3,const float,int_t,int_t,int_t>::type, const float&>::value, "wrong type of indexed array");
        static_assert(std::is_same<make_vrtype<3,float,int_t,placeholder_t,int_t>::type, vec_t<1,float*>>::value, "wrong type of indexed array");
        static_assert(std::is_same<make_vrtype<3,const float,int_t,placeholder_t,int_t>::type, vec_t<1,const float*>>::value, "wrong type of indexed array");
        static_assert(std::is_same<make_vrtype<3,float,int_t,placeholder_t&,int_t>::type, vec_t<1,float*>>::value, "wrong type of indexed array");
        static_assert(std::is_same<make_vrtype<3,float,placeholder_t&,placeholder_t&,placeholder_t&>::type, vec_t<3,float*>>::value, "wrong type of indexed array");
        static_assert(std::is_same<make_vrtype<3,float,vec_t<1, int_t>&,placeholder_t&,placeholder_t&>::type, vec_t<3,float*>>::value, "wrong type of indexed array");
        static_assert(std::is_same<make_vrtype<3,float,const vec_t<1, int_t>&,placeholder_t&,int_t>::type, vec_t<2,float*>>::value, "wrong type of indexed array");
        static_assert(std::is_same<make_vrtype<3,float,vec_t<1, int_t>,placeholder_t&,int_t>::type, vec_t<2,float*>>::value, "wrong type of indexed array");
    }

    {
        // Default initialization of array
        vec1u t = uintarr(3);
        check(t, "0, 0, 0");
    }

    {
        // Index generation 'indgen'
        vec1f v = findgen(7);
        check(v, "0, 1, 2, 3, 4, 5, 6");

        // Direct data access (single index)
        check(v[1], "1");
        check(v[ix(1)], "1");
        // Placeholder index '_'
        check(v[_], "0, 1, 2, 3, 4, 5, 6");

        // Basic math
        check(2*v, "0, 2, 4, 6, 8, 10, 12");
        check(pow(2,v), "1, 2, 4, 8, 16, 32, 64");
        
        // Range generation 'rx'
        vec1i i = rx(5,2);
        check(i, "5, 4, 3, 2");

        // Index array
        check((2*v)[i], "10, 8, 6, 4");
        check(2*v[i], "10, 8, 6, 4");
        v[i] = dindgen(4);
        check(v, "0, 1, 3, 2, 1, 0, 6");
        v[rx(4,6)] = v[rx(2,0)];
        check(v, "0, 1, 3, 2, 3, 1, 0");
        v[i] *= 2;
        check(v, "0, 1, 6, 4, 6, 2, 0");
        v[i] += indgen(4);
        check(v, "0, 1, 9, 6, 7, 2, 0");
        v[rx(0,6)] = v[rx(6,0)];
        check(v, "0, 2, 7, 6, 9, 1, 0");
        check(v[rx(1,4)][rx(1,2)], "7, 6");
        v[ix(1)] = 3;
        check(v, "0, 3, 7, 6, 9, 1, 0");
        check(v(3), "6");
        v[rx(0,6)](0) = 1;
        check(v, "1, 3, 7, 6, 9, 1, 0");
    }

    {
        // Boolean specialization
        vec1b v = {true, false, true, false};
        bool& b1 = v[0];
        b1 = false;
        bool& b2 = v[1];
        b2 = true;
        check(v, "0, 1, 1, 0");
    }
    
    {
        // Initialization list
        vec1f tv = {44, 55, 66, 77};
        check(tv.dims, "4");
        check(n_elements(tv), "4");
        check(tv, "44, 55, 66, 77");
    }
    
    {
        // String array
        vec1s w = {"gn", "gs", "uds", "cosmos"};
        check(w, "gn, gs, uds, cosmos");
    }
    
    {
        // Boolean logic
        vec1i i = indgen(5);
        vec1b b = (i == 3 || i == 1);
        check(b, "0, 1, 0, 1, 0");
        check(!b, "1, 0, 1, 0, 1");

        // 'where' function
        check(where(b), "1, 3");
    }

    {
        // 'reverse' function
        vec1i i = indgen(5);
        check(reverse(i), "4, 3, 2, 1, 0");
    }
    
    {
        // 2 dimensional array & multiple indices '(i,j)'
        vec2d u = dindgen(3,2);
        check(u, "0, 1, 2, 3, 4, 5");
        check(u.dims, "3, 2");
        check(u[ix(0,1)], "1");
        check(u[ix(1,1)], "3");
        check(u(2,1), "5");
        
        u[ix(1,0)] = 9;
        u[ix(2,1)] = 10;
        check(u, "0, 1, 9, 3, 4, 10");

        vec2d tu = {{5,8},{4,5},{1,2}};
        check(tu.dims, "3, 2");
        check(tu.data.size(), "6");
        check(tu, "5, 8, 4, 5, 1, 2");
        check(tu[ix(1,1)], "5");
        tu(rx(1,2),rx(0,1)) = {{1,2},{3,4}};
        check(tu, "5, 8, 1, 2, 3, 4");

        vec2i v = {{4,5,6,7}, {8,9,5,2}, {7,8,2,0}, {-1, -5, -6, -7}, {-4,1,-2,5}};
        check(v[ix(_,_)].dims, "5, 4");
        check(v[ix(_,0)].dims, "5");
        check(v[ix(0,_)].dims, "4");
        check(v[ix(rx(1,2),rx(1,3))].dims, "2, 3");
        check(v[ix(rx(1,2),0)].dims, "2");
        check(v[ix(0,rx(1,3))].dims, "3");
        
        check(v(_,_).dims, "5, 4");
        check(v(_,0).dims, "5");
        check(v(0,_).dims, "4");
        check(v(rx(1,2),rx(1,3)).dims, "2, 3");
        check(v(rx(1,2),0).dims, "2");
        check(v(0,rx(1,3)).dims, "3");
        
        check(v(1,_), "8, 9, 5, 2");
        check(v(2,rx(1,2)), "8, 2");
        check(v(_,1), "5, 9, 8, -5, 1");
        
        v(2,_) = indgen(4);
        check(v, "4, 5, 6, 7, 8, 9, 5, 2, 0, 1, 2, 3, -1, -5, -6, -7, -4, 1, -2, 5");
        v(_,2) = {-4, -4, -4, -4, -4};
        check(v, "4, 5, -4, 7, 8, 9, -4, 2, 0, 1, -4, 3, -1, -5, -4, -7, -4, 1, -4, 5");
    }
    
    {
        // FITS image loading
        vec2d v; fits::header hdr;
        fits::read("data/image.fits", v, hdr);
        check(v.dims, "161, 161");
        check(v(80,79), "0.0191503");
        check(hdr.size()/80, "9");

        // Header functionalities
        uint_t axis;
        check(fits::getkey(hdr, "NAXIS1", axis), "1");
        check(axis, "161");
        check(fits::setkey(hdr, "MYAXIS3", 12), "1");
        check(fits::setkey(hdr, "MYAXIS4", 44), "1");
        check(fits::getkey(hdr, "MYAXIS3", axis), "1");
        check(axis, "12");

        // FITS image writing
        fits::write("out/image_saved.fits", v, hdr);
        vec2d nv;
        fits::read("out/image_saved.fits", nv);
        check(total(nv != v) == 0, "1");
    }
    
    {
        // FITS table loading
        vec1i id1, id2, id3;
        fits::read_table("data/table.fits", ftable(id1, id2, id3));
        check(n_elements(id1), "79003");
        check(n_elements(id2), "79003");
        check(n_elements(id3), "79003");
        check(id1(indgen(10)), "43881, 43881, 43881, 43881, 43548, 43881, 43881, 43881, 43820, 43881");
        
        // FITS table writing
        fits::write_table("out/table_saved.fits", ftable(id1, id2, id3));
        vec1i i1b = id1, i2b = id2, i3b = id3;
        fits::read_table("out/table_saved.fits", ftable(id1, id2, id3));
        check(where(id1 != i1b).empty(), "1");
        check(where(id2 != i2b).empty(), "1");
        check(where(id3 != i3b).empty(), "1");

        // 2-d column
        vec2i test = dindgen(5,2);
        fits::write_table("out/2dtable.fits", ftable(test));
        
        vec2i otest = test;
        fits::read_table("out/2dtable.fits", ftable(test));
        
        check(where(test != otest).empty(), "1");

        // Unsigned type
        vec1u id = uindgen(6);
        fits::write_table("out/utable.fits", ftable(id));
        
        vec1u oid = id;
        fits::read_table("out/utable.fits", ftable(id));
        
        check(where(id != oid).empty(), "1");

        // String type
        vec1s str = {"glouglou", "toto", "tat", "_gqmslk", "1", "ahahahaha"};
        fits::write_table("out/stable.fits", ftable(str));
        
        vec1s ostr = str;
        str = {};
        fits::read_table("out/stable.fits", ftable(str));
        check(where(str != ostr).empty(), "1");
    }

    {
        // FITS table with non vector elements
        int_t i = 5;
        float f = 0.2f;
        std::string s = "toto";
        fits::write_table("out/fits_single.fits", ftable(i, f, s));

        i = 0; f = 0; s = "";
        fits::read_table("out/fits_single.fits", ftable(i, f, s));
        check(i, "5");
        check(f, "0.2");
        check(s, "toto");
    }
    
    {
        // 'match' function
        vec1i t1 = {4,5,6,7,8,9};
        vec1i t2 = {9,1,7,10,5};
        
        vec1i id1, id2;
        match(t1, t2, id1, id2);
        check(id1, "5, 3, 1");
        check(id2, "0, 2, 4");
    }
    
    {
        // ASCII table loading
        vec1i i1, i2;
        file::read_table("data/table.txt", 2, i1, i2);
        check(i1, "0, 1, 2, 3, 4");
        check(i2, "0, 1, 3, 5, 9");
        
        vec1i i2b;
        file::read_table("data/table.txt", 2, _, i2b);
        check(where(i2 != i2b).empty(), "1");
        
        vec2i ids;
        file::read_table("data/table.txt", 2, file::columns(2, ids));
        check(where(i1 != ids(0,_)).empty(), "1");
        check(where(i2 != ids(1,_)).empty(), "1");
        
        vec2i i1c, i2c;
        file::read_table("data/table.txt", 2, file::columns(1, i1c, i2c));
        check(where(i1 != i1c(0,_)).empty(), "1");
        check(where(i2 != i2c(0,_)).empty(), "1");
        
        vec2i i2d;
        file::read_table("data/table.txt", 2, file::columns(1, _, i2d));
        check(where(i2 != i2d(0,_)).empty(), "1");
    }
    
    {
        // File system functions
        check(file::exists("unit_test.cpp"), "1");
        check(file::exists("unit_test_bidouille.cpp"), "0");
        vec1s dlist = file::list_directories("../"); 
        print(dlist);
        check(where(dlist == "bin").empty(), "0");
        vec1s flist = file::list_files();
        print(flist);
        flist = file::list_files("../include/*.hpp");
        print(flist);
    }

    {
        // 'clamp' function
        vec1f f = {-1,0,1,2,3,4,5,6,finf,fnan};
        check(clamp(f, 1, 4), "1, 1, 1, 2, 3, 4, 4, 4, 4, nan");
    }
    
    {
        // 'randomn', 'mean', 'median', 'percentile' functions
        uint_t seed = 42;
        uint_t ntry = 20000;
        vec1d r = randomn(seed, ntry);
        
        double me = mean(r);
        print(me);
        check(fabs(me) < 1.0/sqrt(ntry), "1");
        
        double med = median(r);
        print(med);
        check(fabs(med) < 1.0/sqrt(ntry), "1");
        
        double p1 = percentile(r, 0.158);
        double p2 = percentile(r, 0.842);
        vec1d pi = {p1, p2};
        print(pi);
        check(fabs(p1 + 1.0) < 1.0/sqrt(ntry), "1");
        check(fabs(p2 - 1.0) < 1.0/sqrt(ntry), "1");
        check(percentiles(r, 0.158, 0.842) == pi, "1, 1");
    }
    
    {
        // 'mean' and 'median' for 2-d array
        vec2d v = {{1,2,3},{4,5,6},{7,8,9}};
        check(mean(v,0), "4, 5, 6");
        check(mean(v,1), "2, 5, 8");
        check(median(v,0), "4, 5, 6");
        check(median(v,1), "2, 5, 8");
    }
    
    {
        // 'median' for 3-d array
        vec3d v = dblarr(21,21,11);
        v(_,_,rx(0,4)) = 0;
        v(_,_,rx(6,10)) = 1;
        v(_,_,5) = 0.5;
        
        vec2d m = median(v,2);
        fits::write("out/median.fits", m);
    }

    {
        // 'replicate' function
        vec1i v = replicate(3, 5);
        check(v.dims, "5");
        check(v, "3, 3, 3, 3, 3");
    }
    
    {
        // 'transpose' & 'circular_mask' function
        vec1u dim = {51,41};
        vec2d px = replicate(dindgen(dim(1)), dim(0));
        vec2d py = transpose(replicate(dindgen(dim(0)), dim(1)));
        fits::write("out/px.fits", px);
        fits::write("out/py.fits", py);
    
        vec2d m = circular_mask({51,51}, {25,25}, 8);
        fits::write("out/circular.fits", m);
    }

    {
        // 'enlarge' function
        vec2d v = dblarr(5,5)*0 + 3.1415;
        v = enlarge(v, 5, 1.0);
        fits::write("out/enlarge.fits", v);
    }
    
    {
        // 'subregion' function
        vec2i v = indgen(5,5);
        fits::write("out/sub0.fits", v);
        
        vec2i s = subregion(v, {0,0,4,4});
        vec2i tv = v;
        check(total(s != tv) == 0, "1");
        fits::write("out/sub1.fits", s);
        
        s = subregion(v, {1,0,3,4});
        tv = {{1,2,3},{6,7,8},{11,12,13},{16,17,18},{21,22,23}};
        check(total(s != tv) == 0, "1");
        fits::write("out/sub2.fits", s);
        
        s = subregion(v, {-1,1,5,3});
        tv = {{0,5,6,7,8,9,0},{0,10,11,12,13,14,0},{0,15,16,17,18,19,0}};
        check(total(s != tv) == 0, "1");
        fits::write("out/sub4.fits", s);
    }
    
    {
        // 'rgen' function
        vec1d v = rgen(0, 5, 11);
        check(v, "0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5");
    }

    {
        // 'merge' function
        vec1i v = {1, 2, 3, 4};
        v = merge(0, v);
        check(v, "0, 1, 2, 3, 4");
        v = merge(v, 5);
        check(v, "0, 1, 2, 3, 4, 5");
        vec1i t = {4, 3, 2, 1};
        v = merge(v, t);
        check(v, "0, 1, 2, 3, 4, 5, 4, 3, 2, 1");
    }

    {
        // 'append' & 'prepend' functions
        vec1i v = {0, 5, 2};
        vec1i w = {4, 1, 3};
        append(v, w);
        check(v, "0, 5, 2, 4, 1, 3");
        prepend(v, w);
        check(v, "4, 1, 3, 0, 5, 2, 4, 1, 3");
    }

    {
        // 'append' for 2-d array
        vec2i v = {{0,1,2}, {5,6,8}};
        vec2i w = {{5,5,5}};
        append<0>(v, w);
        check(v(0,_), "0, 1, 2");
        check(v(1,_), "5, 6, 8");
        check(v(2,_), "5, 5, 5");
    }
    
    {
        // Calculus functions
        check(derivate1([](const vec1d& x) { return cos(x(0)); }, 1.0, 0.001, 0), "-0.841471");
        check(derivate2([](const vec1d& x) { return cos(x(0)); }, 1.0, 0.001, 0), "-0.540302");

        vec2d a = {
            {1.000, 2.000, 3.000},
            {4.000, 1.000, 6.000},
            {7.000, 8.000, 1.000}
        };
        
        vec2d id = {{1,0,0},{0,1,0},{0,0,1}};
        
        vec2d i;
        check(invert(a,i), "1");
        vec2d tid = mmul(a,i);
        mprint(tid);
        check(stddev(tid - id) < 1e-10, "1");
        
        vec2i sq = {{1,1,1},{1,1,1},{1,1,1}};
        diag(sq) *= 5;
        check(sq, "5, 1, 1, 1, 5, 1, 1, 1, 5");

        vec1d x = {0,1,2};
        vec1d y = {0,1,0};
        check(integrate(x,y), "1");
        
        x = 0.5*3.14159265359*dindgen(30)/29.0;
        y = cos(x);
        check(fabs(1.0 - integrate(x,y)) < 0.001, "1");
        
        check(integrate([](float t) -> float {
                return (2.0/sqrt(3.14159))*exp(-t*t);
            }, 0.0, 1.0), strn(erf(1.0))
        );
    }
    
    {
        // 'linfit' function
        vec1d x = dindgen(5);
        vec1d y = 3.1415*x*x - 12.0*x;

        auto fr = linfit(y, 1.0, 1.0, x, x*x);
        if (fr.success) {
            check(abs(fr.params(0)) < 1e-6, "1");
            check(fr.params(1), "-12");
            check(fr.params(2), "3.1415");
        } else {
            print("failure...");
            print("param: ", fr.params);
            print("error: ", fr.errors);
            print("chi2: ", fr.chi2);
            check(fr.success, "1");
        }
    }

    {
        // 'linfit' function doing PSF fitting
        vec2d img, psf;
        fits::read("data/stack1.fits", img);
        fits::read("data/psf.fits", psf);

        auto fr = linfit(img, 1.0, 1.0, psf);
        if (fr.success) {
            check(fr.params(0), "21.6113");
            check(fr.params(1), "90.4965");
        } else {
            print("params: ", fr.params);
            print("errors: ", fr.errors);
            print("chi2: ", fr.chi2);
            assert(false);
        }
        
        img -= psf*fr.params(1);
        fits::write("out/residual.fits", img);
    }
    
    {
        // 'affinefit' function
        vec1d x = dindgen(5);
        vec1d y = 5*x + 3;
        vec1d ye = y*0 + 1;

        auto fr = affinefit(x, y, ye);
        check(fr.slope, "5");
        check(fr.offset, "3");
    }

    {
        // 'nlfit' function
        vec1d x = dindgen(5);
        vec1d y = 3.1415*x*x;

        auto model = [&](const vec1d& p) { return p(0)*x*x; };

        auto fr = nlfit(model, y, 1.0, {1.0});
        if (fr.success) {
            check(fr.params, "3.1415");
        } else {
            print("failure: ", fr.status);
            print("param: ", fr.params);
            print("error: ", fr.errors);
            print("chi2: ", fr.chi2);
            print("iter: ", fr.niter);
            assert(false);
        }
    }
    
    {
        // 'sort' function
        vec1i v = {6,2,4,3,1,5};
        vec1i sid = sort(v);
        check(sid, "4, 1, 3, 2, 5, 0");
        check(v[sid], "1, 2, 3, 4, 5, 6");
    }

    {
        // 'sort' function again
        vec2i v = {{5,0}, {4,1}, {1,2}, {2,3}};
        vec1i sid = sort(v(_,0));
        v(_,_) = v(sid,_);
        check(v, "1, 2, 2, 3, 4, 1, 5, 0");
    }
    
    {
        // 'replace' & 'split'
        vec1s s = {"/home/dir/", "/bin/ds9", "ls"};
        s = replace(s, "/", ":");
        check(s, ":home:dir:, :bin:ds9, ls");

        vec1s t = split(s[0], ":");
        check(t, ", home, dir, ");
    }

    {
        // Cosmology functions
        auto cosmo = cosmo_wmap();
        check(lumdist(0.5, cosmo), "2863.03");
        check(lookback_time(0.5, cosmo), "5.09887");

        vec1d v = rgen(0.0, 12.0, 1500);
        auto r = lumdist(v, cosmo);
        auto r1 = merge(lumdist(v[rgen(0,750)], cosmo), lumdist(v[rgen(751,1499)], cosmo));
        check(max(fabs(r - r1)/r1) < 1e-5, "1");
    }

    {
        // 'interpol', 'lower_bound' & 'upper_bound' functions
        vec1f y = {0, fnan, 2, 3, 4, 5};
        vec1f x = {0, 1, 2, 3, 4, 5};
        vec1f nx = {0.2, 0.5, 1.6, -0.5, 12};
        vec1f i = interpol(y, x, nx);
        float t = 0.5;
        check(y(lower_bound(t, y)), "0");
        check(y(upper_bound(t, y)), "2");
        check(total(i != nx), "0");
    }

    {
        // 'uJy2lsun' & 'lsun2uJy' functions
        vec1d jy = {1000.0};
        double z = 1.0;
        double d = lumdist(z, cosmo_wmap());
        double lam_obs = 160.0;
        auto lsun = uJy2lsun(z, d, lam_obs, jy);
        double lam_rf = lam_obs/(1.0 + z);
        check(jy == lsun2uJy(z, d, lam_rf, lsun), "1");
    }

    {
        // Reflection: print a structure
        struct {
            int_t i = 5;
            vec1f j = indgen(3);
            struct {
                int_t u = -1;
                int_t v = 2;
            } k;
        } str;

        check(str, "{ i=5, j=[0, 1, 2], k={ u=-1, v=2 } }");
    }

    {
        // Reflection: save & write a structure
        struct tmp_t {
            vec1d f = indgen(3);
            vec1i s = {5,4,2,3,1};
            struct {
                vec1i u = {1,2};
                vec1i v = {3,4};
            } t;
        };

        tmp_t tmp;
        tmp.f = {5,4,8,9};
        tmp.s = {1,2};
        std::swap(tmp.t.u, tmp.t.v);

        tmp_t tmp2;

        fits::write_table("out/reflex_tbl.fits", tmp);
        fits::read_table("out/reflex_tbl.fits", tmp2);

        check(tmp2, strn(tmp));
    }

    {
        // Reflection: merge two structures
        struct {
            int_t i = 0;
            float j = 0;
            struct {
                float k = 0;
                std::string v = "";
                int_t w = 0;
            } t;
        } tmp1;

        struct {
            int_t i = 5;
            struct {
                std::string v = "toto";
            } t;
            struct {
                int_t z = 12;
            } p;
        } tmp2;


        struct ut {
            struct utt { int_t i, j; } t;
        } tmp3;

        merge_elements(tmp1, tmp2);

        check(tmp1, "{ i=5, j=0, t={ k=0, v=toto, w=0 } }");
    }
    
    print("\nall tests passed!\n");
    
    return 0;
}