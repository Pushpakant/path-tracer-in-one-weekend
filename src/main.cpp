//Compile: g++ -g -Wshadow -Wall main.cpp -o a.exe -Ofast -Wno-unused-result && a.exe
#include <cstdio>
#include <iostream>

#include "../include/camera.h"
#include "../include/color.h"
#include "../include/hittable_list.h"
#include "../include/image.h"
#include "../include/ray.h"
#include "../include/rtweekend.h"
#include "../include/sphere.h"
#include "../include/vec3.h"

double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();         //  a = b*b
    auto half_b = dot(oc, r.direction());            //  b = 2*b*(a-c)
    auto c = oc.length_squared() - radius * radius;  //  c = (a-c)*(a-c) - r^2
    auto discriminant = half_b * half_b - a * c;

    if (discriminant < 0) return -1.0;

    return (-half_b - sqrt(discriminant)) / a;
}

color ray_color(const ray& r, const hittable& world) {
    hit_record rec;

    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }

    vec3 unit_direction = unit_vector(r.direction());  //  -1.0<=y<=1
    auto t = 0.5 * (unit_direction.y() + 1.0);

    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);  //  linear interpolation
}

int32_t main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::cout.tie(NULL);

    freopen("img.ppm", "w", stdout);
    //  image

    double aspect_ratio = 16.0 / 9.0;
    int imgWidth = 500;
    Image<int> img(imgWidth, static_cast<int>(imgWidth / aspect_ratio));
    const int sample_per_pixel = 10;

    //  world
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    //  camera
    camera cam;

    //  render

    std::cout << "P3\n"
              << img.getWidth() << ' ' << img.getHeight() << "\n255\n";

    for (int j = img.getHeight() - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < img.getWidth(); ++i) {
            color pixel_color;
            for (int s = 0; s < sample_per_pixel; ++s) {
                auto u = (i + random_double()) / (img.getWidth() - 1);
                auto v = (j + random_double()) / (img.getHeight() - 1);

                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world);
            }

            write_color(std::cout, pixel_color, sample_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}