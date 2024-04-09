#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
  double spaceX = width / (num_width_points - 1);
  double spaceY = height / (num_height_points - 1);
  for (int y = 0; y < num_height_points; y++) {
    for (int x = 0; x < num_width_points; x++) {
      Vector3D pos;
      if (orientation != HORIZONTAL) {
        pos = Vector3D(x * spaceX, y * spaceY, (rand() % 2001 - 1000) / 1000.0 / 1000.0);
      } else {
        pos = Vector3D(x * spaceX, 1, y * spaceY);
      }

      bool isPointPinned = false;
      for (const auto& pin : pinned) {
        if (pin[0] == x && pin[1] == y) {
          isPointPinned = true;
          break;
        }
      }
      point_masses.emplace_back(pos, isPointPinned);
    }
  }

  for (int y = 0; y < num_height_points; y++) {
    for (int x = 0; x < num_width_points; x++) {
      int i = y * num_width_points + x;
      PointMass* pm = &point_masses[i];
      // Structural
      if (x > 0) springs.emplace_back(pm, &point_masses[i - 1], STRUCTURAL);
      if (y > 0) springs.emplace_back(pm, &point_masses[i - num_width_points], STRUCTURAL);
      // Shearing
      if (x > 0 && y > 0) springs.emplace_back(pm, &point_masses[i - num_width_points - 1], SHEARING);
      if (x < num_width_points - 1 && y > 0) springs.emplace_back(pm, &point_masses[i - num_width_points + 1], SHEARING);
      // Bending
      if (x > 1) springs.emplace_back(pm, &point_masses[i - 2], BENDING);
      if (y > 1) springs.emplace_back(pm, &point_masses[i - 2 * num_width_points], BENDING);
    }
  }
}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2): Compute total force acting on each point mass.
  Vector3D total_force(0, 0, 0);
  for (Vector3D &acceleration : external_accelerations) {
    total_force += acceleration * mass; // F = ma
  }

  for (PointMass &pm : point_masses) {
    pm.forces = Vector3D(0, 0, 0);
    pm.forces += total_force;
  }

  for (Spring &spring : springs) {
    if ((spring.spring_type == STRUCTURAL && cp->enable_structural_constraints) || (spring.spring_type == SHEARING && cp->enable_shearing_constraints) || (spring.spring_type == BENDING && cp->enable_bending_constraints)) {
      Vector3D diff = spring.pm_a->position - spring.pm_b->position;
      double force_mag = cp->ks * (diff.norm() - spring.rest_length);
      if (spring.spring_type == BENDING) {
        force_mag *= 0.2;
      }
      Vector3D spring_force = diff.unit() * force_mag;
      spring.pm_a->forces -= spring_force;
      spring.pm_b->forces += spring_force;
    }
  }

  // TODO (Part 2): Use Verlet integration to compute new point mass positions
  for (PointMass &pm : point_masses) {
    if (!pm.pinned) {
      Vector3D acceleration = pm.forces / mass;
      Vector3D last_pos = pm.position;
      pm.position += (1 - cp->damping / 100) * (pm.position - pm.last_position) + acceleration * pow(delta_t, 2);
      pm.last_position = last_pos;
    }
  }

  // TODO (Part 4): Handle self-collisions.
  build_spatial_map();
  for (PointMass &pm : point_masses) {
    self_collide(pm, simulation_steps);
  }

  // TODO (Part 3): Handle collisions with other primitives.
  for (PointMass &pm: point_masses) {
    for (auto &obj: *collision_objects) {
      obj->collide(pm);
    }
  }

  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
  for (Spring &spring : springs) {
    Vector3D diff = spring.pm_a->position - spring.pm_b->position;
    double len = diff.norm();
    if (len > spring.rest_length * 1.1) {
      Vector3D correction = (len - spring.rest_length * 1.1) * diff.unit();
      if (!spring.pm_a->pinned && !spring.pm_b->pinned) {
        spring.pm_a->position -= correction / 2;
        spring.pm_b->position += correction / 2;
      } else if (spring.pm_a->pinned) {
        spring.pm_b->position += correction;
      } else if (spring.pm_b->pinned) {
        spring.pm_a->position -= correction;
      }
    }
  }
}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.
  for (PointMass &pm : point_masses) {
    float hash_pos = hash_position(pm.position);
    if (!map.count(hash_pos)) {
      map[hash_pos] = new vector<PointMass*>;
    }
    map[hash_pos]->push_back(&pm);
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
  float hash_pos = hash_position(pm.position);
  if (map.count(hash_pos)) {
    Vector3D correction(0, 0, 0);
    int count = 0;
    for (PointMass *other_pm : *map[hash_pos]) {
      if (other_pm != &pm) {
        Vector3D diff = pm.position - other_pm->position;
        float dist = diff.norm();
        if (2 * thickness > dist) {
          correction += diff.unit() * (2 * thickness - dist);
          count++;
        }
      }
    }
    if (0 < count) {
      pm.position += correction * (1.0 / (count * simulation_steps));
    }
  }
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
  float gridWidth = 3 * width / num_width_points;
  float gridHeight = 3 * height / num_height_points;
  float largestDim = std::max(gridWidth, gridHeight);

  float x = floor(pos.x / gridWidth) * gridWidth;
  float y = floor(pos.y / gridHeight) * gridHeight;
  float z = floor(pos.z / largestDim) * largestDim;

  size_t hashedX = std::hash<float>{}(x);
  size_t hashedY = std::hash<float>{}(y) << 1;
  size_t hashedZ = std::hash<float>{}(z) << 2;

  return static_cast<float>(hashedX ^ hashedY ^ hashedZ);
  // return 0.f; 
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
