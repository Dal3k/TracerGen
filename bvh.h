#ifndef TRACERGEN_BVH_H
#define TRACERGEN_BVH_H

#include <algorithm>

#include "utility.h"
#include "hittable.h"
#include "hittable_list.h"
#include "aabb.h"

class bvh_node : public hittable {
public:
    bvh_node();

    bvh_node(const hittable_list &list, double time0, double time1)
            : bvh_node(list.objects, 0, list.objects.size(), time0, time1) {}

    bvh_node(const std::vector<shared_ptr<hittable>> &src_objects,
             size_t start, size_t end, double time0, double time1);

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;

    virtual bool bounding_box(double time0, double time1, aabb &output_box) const override;

public:
    std::array<shared_ptr<hittable>, 2> children;
    aabb box;
};

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
    aabb box_a;
    aabb box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min().e[axis] < box_b.min().e[axis];
}


bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
}

bvh_node::bvh_node(
        const std::vector<shared_ptr<hittable>> &src_objects,
        size_t start, size_t end, double time0, double time1
) {
    auto objects = src_objects; // Create a modifiable array of the source scene objects

    aabb full_box;
    for (size_t i = start; i < end; i++) {
        aabb temp_box;
        if (!objects[i]->bounding_box(time0, time1, temp_box))
            std::cerr << "No bounding box in bvh_node constructor.\n";
        full_box = surrounding_box(full_box, temp_box);
    }

    int axis = full_box.longest_axis();
    auto comparator = (axis == 0) ? box_x_compare
                                  : (axis == 1) ? box_y_compare
                                                : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        children[0] = children[1] = objects[start];
    } else if (object_span == 2) {
        children[0] = objects[start + (comparator(objects[start], objects[start + 1]) ? 0 : 1)];
        children[1] = objects[start + (comparator(objects[start], objects[start + 1]) ? 1 : 0)];
    } else {
        // Implement Surface Area Heuristic (SAH) for BVH construction
        std::vector<aabb> left_boxes(object_span);
        std::vector<aabb> right_boxes(object_span);
        aabb left_box, right_box;

        for (size_t i = start; i < end; i++) {
            aabb temp_box;
            if (!objects[i]->bounding_box(time0, time1, temp_box))
                std::cerr << "No bounding box in bvh_node constructor.\n";
            left_box = surrounding_box(left_box, temp_box);
            left_boxes[i - start] = left_box;
        }

        for (size_t i = end; i > start; i--) {
            aabb temp_box;
            if (!objects[i - 1]->bounding_box(time0, time1, temp_box))
                std::cerr << "No bounding box in bvh_node constructor.\n";
            right_box = surrounding_box(right_box, temp_box);
            right_boxes[i - start - 1] = right_box;
        }

        double min_cost = std::numeric_limits<double>::infinity();
        size_t split_index = start;

        for (size_t i = start; i < end - 1; i++) {
            double left_area = left_boxes[i - start].area();
            double right_area = right_boxes[i - start + 1].area();
            double cost = (i - start + 1) * left_area + (end - i - 1) * right_area;

            if (cost < min_cost) {
                min_cost = cost;
                split_index = i + 1;
            }
        }

        // Use oneTBB's parallel_invoke to construct child nodes in parallel
        tbb::parallel_invoke(
            [&] { children[0] = make_shared<bvh_node>(objects, start, split_index, time0, time1); },
            [&] { children[1] = make_shared<bvh_node>(objects, split_index, end, time0, time1); }
        );

        // Reorder child nodes for better cache usage during traversal
        aabb child_box0, child_box1;
        if (!children[0]->bounding_box(time0, time1, child_box0) || !children[1]->bounding_box(time0, time1, child_box1))
            std::cerr << "No bounding box in bvh_node constructor.\n";

        if (child_box0.area() > child_box1.area()) {
            std::swap(children[0], children[1]);
        }
    }

    aabb box_left, box_right;
    if (!children[0]->bounding_box(time0, time1, box_left)
        || !children[1]->bounding_box(time0, time1, box_right)
            )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = surrounding_box(box_left, box_right);
}

bool bvh_node::bounding_box(double time0, double time1, aabb &output_box) const {
    output_box = box;
    return true;
}

bool bvh_node::hit(const ray &r, double t_min, double t_max, hit_record &rec) const {
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = children[0]->hit(r, t_min, t_max, rec);
    bool hit_right = children[1]->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}

#endif //TRACERGEN_BVH_H
