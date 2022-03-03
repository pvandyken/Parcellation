#include "intersection.h"

// ============ Producto Punto =============
const float Intersection::dotProduct(const float a[], float *&b)
{
  float c = 0;

#pragma omp simd reduction(+ \
                           : c)
  for (int i = 0; i < 3; i++)
    c += a[i] * b[i];

  return c;
}

// ============ Producto Cruz ==============
float *Intersection::crossProduct(const float a[], const float b[])
{
  float *c = new float[3];
  int i = 1, j = 2;

#pragma omp simd
  for (int k = 0; k < 3; k++)
  {
    c[k] = a[i] * b[j] - a[j] * b[i];

    i = (i + 1) % 3;
    j = (j + 1) % 3;
  }

  return c;
}

// =========== Función que calcula la intersección entre un rayo con un
// triángulo ====================
bool Intersection::ray_triangle_intersection(const float ray_near[], const float ray_dir[],
                                             const float Points[][3], float &t)
{

  const float eps = 0.000001;
  float edges[2][3];

#pragma omp simd collapse(2)
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      edges[i][j] = Points[i + 1][j] - Points[0][j];
    }
  }

  float *pvec = crossProduct(ray_dir, edges[1]);
  const float det = dotProduct(edges[0], pvec);

  if (fabs(det) < eps)
  {
    delete[] pvec;
    return false;
  }

  const float inv_det = 1. / det;

  float tvec[3];
#pragma omp simd
  for (int i = 0; i < 3; i++)
    tvec[i] = ray_near[i] - Points[0][i];

  const float u = dotProduct(tvec, pvec) * inv_det;
  delete[] pvec;

  if ((u < 0.) || (u > 1.))
    return false;

  float *qvec = crossProduct(tvec, edges[0]);
  const float v = dotProduct(ray_dir, qvec) * inv_det;

  if ((v < 0.) || (u + v > 1.))
  {
    delete[] qvec;
    return false;
  }

  t = dotProduct(edges[1], qvec) * inv_det;
  delete[] qvec;

  if (t < eps)
    return false;

  return true;
}

vector<vector<float>> Intersection::multiple_vertices(const float *triangle[3])
{

  vector<vector<float>> pt(6, vector<float>(3));
  // float **pt = new float*[6];
  // for(int i=0; i<6; i++)
  // pt[i] = new float[3];

  for (int i = 0; i < 3; i++)
  {
    pt[0][i] = triangle[0][i];
    pt[2][i] = triangle[1][i];
    pt[4][i] = triangle[2][i];

    pt[1][i] = (triangle[0][i] + triangle[1][i]) / 2.0;
    pt[3][i] = (triangle[1][i] + triangle[2][i]) / 2.0;
    pt[5][i] = (triangle[0][i] + triangle[2][i]) / 2.0;
  }

  return pt;
}

vector<vector<vector<float>>>
Intersection::multiple_triangles(const float *triangles[1][3], int &len, const int polys[][3])
{
  vector<vector<vector<float>>> new_triangles(
      len * 4, vector<vector<float>>(3, vector<float>(3)));

  for (int i = 0; i < len; i++)
  {
    vector<vector<float>> tri = multiple_vertices(triangles[i]);

    for (int j = 0; j < 4; j++)
    {
      // new_triangles[i*4 + j] = new float*[3];

      for (int k = 0; k < 3; k++)
      {
        // new_triangles[i*4 + j][k] = new float[3];

        for (int l = 0; l < 3; l++)
          new_triangles[i * 4 + j][k][l] = tri[polys[j][k]][l];
      }
    }
  }
  len = len * 4;
  return new_triangles;
}

vector<vector<vector<float>>> Intersection::segment_triangles(const float *triangles[1][3])
{
  /*
  Split each triangle into four sub triangles
  and finds the centroid of each sub-triangle
       ^           ^
      / \         /.\
     /   \  -->  /---\
    /     \     /.\•/.\
   <_______>   <___|___>
  */
  int len = 1;
  int polys[4][3] = {{0, 1, 5}, {1, 2, 3}, {5, 3, 4}, {1, 3, 5}};

  return multiple_triangles(triangles, len, polys);
}

vector<vector<float>>
Intersection::get_triangle_centroid(vector<vector<vector<float>>> const triangles,
                                    const int &N)
{
  vector<vector<float>> centroid(int(pow(4, N)), vector<float>(3));
  int len = 1;
  for (int i = 0; i < len; i++)
  {

    for (int j = 0; j < 3; j++)
    {
      float sum = 0;

#pragma omp simd reduction(+ \
                           : sum)
      for (int k = 0; k < 3; k++)
        sum += triangles[i][k][j];

      centroid[i][j] = sum / 3.0;
    }
  }
  return centroid;
}

const bool CorticalIntersection::getMeshAndFiberEndIntersection(
    Vector3f &fiberP0, Vector3f &fiberP1, const int &nPoints,
    const int &nPtsLine, const int &N, const int &npbp,
    vector<vector<float>> &index, const float &step,
    vector<vector<vector<bool>>> &cubeNotEmpty,
    const vector<vector<vector<vector<int>>>> &centroidIndex,
    const vector<vector<vector<vector<vector<float>>>>> &almacen,
    const EigenDRef<const MatrixX3f> &vertex, const EigenDRef<const MatrixX3i> &polygons,
    int &Ind, vector<float> &ptInt)
{

  Vector3f dd = (fiberP0 - fiberP1) / (npbp + 1);

  vector<Vector3i> indexes;
  Vector3f gridStart(index[0][0], index[1][0], index[2][0]);

  for (int i = 0; i <= nPtsLine + (nPtsLine - 1) * npbp; i++)
  {

    Vector3i I = ((fiberP1 + i * dd - gridStart) / step).cast<int>();

    // Check all neighboring cubes
#pragma omp simd collapse(3)
    for (int a = -1; a < 2; a++)
    {
      for (int b = -1; b < 2; b++)
      {
        for (int c = -1; c < 2; c++)
        {
          if (cubeNotEmpty[I[0] + a][I[1] + b][I[2] + c])
          {
            Vector3i abc(a, b, c);
            indexes.emplace_back(I + abc);
          }
        }
      }
    }
  }

  if (indexes.empty())
    return false;

  sort(indexes.begin(), indexes.end(), &Utils::compareVec<Vector3i>);
  indexes.erase(unique(indexes.begin(), indexes.end()), indexes.end());
  vector<vector<double>> listDist;

  for (const auto &I : indexes)
  {
    for (int u = 0; u < (int)centroidIndex[I[0]][I[1]][I[2]].size(); u++)
    {
      double cen[3], dist = 0;

#pragma omp simd reduction(+ \
                           : dist)
      for (int i = 0; i < 3; i++)
      {
        cen[i] = almacen[I[0]][I[1]][I[2]][u][i];
        dist += fabs(cen[i] - fiberP0[i]);
      }

      const int &c_index = centroidIndex[I[0]][I[1]][I[2]][u];
      listDist.emplace_back((vector<double>){dist, (double)c_index});
    }
  }
  sort(listDist.begin(), listDist.end());
  vector<int> listIndex;
  ptInt = vector<float>(3);

  for (const vector<double> &ind : listDist)
  {
    if (find(listIndex.begin(), listIndex.end(), (int)ind[1]) ==
        listIndex.end())
      listIndex.emplace_back((int)ind[1]);
    else
      continue;

    float ray_dir[3], ray_near[3];

#pragma omp simd
    for (int i = 0; i < 3; i++)
    {
      ray_dir[i] = fiberP1[i] - fiberP0[i];
      ray_near[i] = fiberP1[i];
    }

    const int *Triangle = polygons((int)ind[1], all).data();
    float Pts[3][3];

#pragma omp simd collapse(2)
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        Pts[i][j] = vertex(Triangle[i], j);
      }
    }

    float t;
    // ========== Verifica la intersección entre el rayo y el triángulo
    // =============
    if (Intersection::ray_triangle_intersection(ray_near, ray_dir, Pts, t))
    {

#pragma omp simd
      for (int i = 0; i < 3; i++)
        ptInt[i] = fiberP1[i] + (fiberP1[i] - fiberP0[i]) * t;

      Ind = (int)ind[1];
      return true;
    }
    else
    {
      // ============= Ídem, pero con el rayo apuntando hacia el sentido
      // contrario =============

      float ray_invert[3];
#pragma omp simd
      for (int i = 0; i < 3; i++)
        ray_invert[i] = -ray_dir[i];

      if (Intersection::ray_triangle_intersection(ray_near, ray_invert, Pts, t))
      {

#pragma omp simd
        for (int i = 0; i < 3; i++)
          ptInt[i] = fiberP1[i] - (fiberP1[i] - fiberP0[i]) * t;

        Ind = (int)ind[1];
        return true;
      }
    }
  }
  return false;
}

const bool CorticalIntersection::getMeshAndFiberIntersection(
    vector<Vector3f> &fiber, const int &nPoints, const int &nPtsLine,
    const int &N, const int &npbp, vector<vector<float>> &index,
    const float &step, vector<vector<vector<bool>>> &cubeNotEmpty,
    const vector<vector<vector<vector<int>>>> &centroidIndex,
    const vector<vector<vector<vector<vector<float>>>>> &almacen,
    const EigenDRef<const MatrixX3f> &vertex, const EigenDRef<const MatrixX3i> &polygons, int &InInd, int &FnInd,
    vector<float> &InPtInt, vector<float> &FnPtInt)
{

  bool findInt;

  findInt = getMeshAndFiberEndIntersection(
      fiber[0], fiber[1], nPoints, nPtsLine, N, npbp, index, step, cubeNotEmpty,
      centroidIndex, almacen, vertex, polygons, InInd, InPtInt);

  if (!findInt)
    return false;

  findInt = getMeshAndFiberEndIntersection(
      fiber[nPoints - 1], fiber[nPoints - 2], nPoints, nPtsLine, N, npbp, index,
      step, cubeNotEmpty, centroidIndex, almacen, vertex, polygons, FnInd,
      FnPtInt);

  if (!findInt)
  {
    return false;
  }

  return true;
}

CorticalIntersection::CorticalIntersection(
    const Mesh &mesh,
    vector<Bundle> &bundles, const int &nPtsLine)
    : mesh{mesh},
      frontIntersections{vector<BundleIntersection>(bundles.size())},
      backIntersections{vector<BundleIntersection>(bundles.size())},
      fibIndex{vector<vector<int>>(bundles.size())}
{
  const EigenDRef<const MatrixX3f> &vertex = mesh.vertices;
  const EigenDRef<const MatrixX3i> &polygons = mesh.polygons;

  cout << "Calculate mesh and bundles intersection" << endl;
  int N = 1;

  vector<float> mdbvs(polygons.rows());
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < polygons.rows(); i++)
  {
    const Vector3f pts[3] = {
        vertex(polygons(i, 0), all),
        vertex(polygons(i, 1), all),
        vertex(polygons(i, 2), all)};

    const float dists[3] = {
        (pts[0] - pts[1]).norm(),
        (pts[1] - pts[2]).norm(),
        (pts[2] - pts[0]).norm()};

    mdbvs[i] = *max_element(begin(dists), end(dists));
  }
  float mdbv = *max_element(begin(mdbvs), end(mdbvs));
  const float step = mdbv * 1.5 / pow(2, N + 1);

  cout << "Calculate maximal size between points..." << endl;
  float mdbp = 0; // maximum distance between points
  for (int i = 0; i < (int)bundles.size(); i++)
  {
    for (int j = 0; j < (int)bundles[i].fibers.size(); j++)
    {

      float dist = 0;

      for (int k = 0; k < 3; k++)
        dist += pow(bundles[i].fibers[j][0][k] - bundles[i].fibers[j][1][k], 2);

      dist = sqrt(dist);
      if (dist > mdbp)
        mdbp = dist;

      dist = 0;
      for (int k = 0; k < 3; k++)
        dist += pow(bundles[i].fibers[j][0][k] - bundles[i].fibers[j][1][k], 2);

      dist = sqrt(dist);
      if (dist > mdbp)
        mdbp = dist;
    }
  }

  int npbp = mdbp / step; // number of points between points

  vector<float> vx(vertex.rows());
  vector<float> vy(vertex.rows());
  vector<float> vz(vertex.rows());
// ===== Find outermost vertex ===========
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < vertex.rows(); i++)
  {
    vx[i] = vertex(i, 0);
    vy[i] = vertex(i, 1);
    vz[i] = vertex(i, 2);
  }

  sort(vx.begin(), vx.end());
  sort(vy.begin(), vy.end());
  sort(vz.begin(), vz.end());

  const float minx =
      *vx.begin() - (nPtsLine + 1) * mdbp - 4 * step; // coordenada x minima
  const float maxx =
      *vx.rbegin() + (nPtsLine + 1) * mdbp + 4 * step; // coordenada x maxima
  const float miny =
      *vy.begin() - (nPtsLine + 1) * mdbp - 4 * step; // coordenada y minima
  const float maxy =
      *vy.rbegin() + (nPtsLine + 1) * mdbp + 4 * step; // coordenada y maxima
  const float minz =
      *vz.begin() - (nPtsLine + 1) * mdbp - 4 * step; // coordenada z minima
  const float maxz =
      *vz.rbegin() + (nPtsLine + 1) * mdbp + 4 * step; // coordenada z maxima

  const Vector3f minCoord = Vector3f(minx, miny, minz);
  const Vector3f maxCoord = Vector3f(maxx, maxy, maxz);

  // Filter bundles to get only fibres who's first and last points are within
  // the bounding box of the surface
  cout << "Filter bundles..." << endl;

  const Array3f minFiberBound = (Vector3f(vx.front(), vy.front(), vz.front()).array() - mdbp - (2 * step));
  const Array3f maxFiberBound = (Vector3f(vx.back(), vy.back(), vz.back()).array() + mdbp + (2 * step));

  for (auto &bundle : bundles)
  {
    // Loop through bundle, erasing fibres that don't fit
    for (auto fibre = bundle.fibers.begin(); fibre != bundle.fibers.end();)
    {
      const auto init_pt = fibre->front().array();
      const auto final_pt = fibre->back().array();
      const auto initial_within = ((minFiberBound <= init_pt).all() && (maxFiberBound >= init_pt).all());
      const auto final_within = ((minFiberBound <= final_pt).all() && (maxFiberBound >= final_pt).all());

      if (!initial_within || !final_within)
      {
        bundle.fibers.erase(fibre);
      }
      else
      {
        fibre++;
      }
    }
  }

  cout << "Get number of intervals per axis..." << endl;
  // ================ Obtiene la cantidad de intervalos por eje
  //  Get the number of intervals per axix
  const Vector3f mins = Vector3f(minx, miny, minz);
  const Vector3f maxs = Vector3f(maxx, maxy, maxz);
  const Vector3i counts = ((maxs - mins) / step).cast<int>();

  // ====== Generacion de intervalos (coordenadas de los vertices de cada cubo)
  //  Interval Generation (coordinates of the vertices of each cube)
  cout << "Generate intervals..." << endl;
  vector<vector<float>> index(3, vector<float>());

  for (int i = 0; i < 3; i++)
  {
    vector<float> indexi(counts[i] + 1);
    index[i] = indexi;
    // index[i] = new float[counts[i] + 1];

#pragma omp simd
    for (int j = 0; j < counts[i] + 1; j++)
    {
      index[i][j] = mins[i] + j * step;
    }
  }

  vector<vector<float>> centroids(polygons.rows() * pow(4, N), vector<float>(3));

  cout << "Segmenting polygons..." << endl;
#pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < polygons.rows(); i++)
  {
    const float *triangles[1][3];
    // triangles[0] = new float*[3];

    for (int j = 0; j < 3; j++)
      triangles[0][j] = vertex(polygons(i, j), all).data();

    vector<vector<vector<float>>> subTriangles = Intersection::segment_triangles(triangles);
    vector<vector<float>> centroid = Intersection::get_triangle_centroid(subTriangles, N);

    for (int j = 0; j < pow(4, N); j++)
    {
      for (int k = 0; k < 3; k++)
        centroids[i * pow(4, N) + j][k] = centroid[j][k];
    }
  }

  // Characterize the space as a 3 dimensional field composed of cubes with edge
  // length of step. Then assign each centroid to this field according to the
  // cube they are in
  //
  //   x[     y[     z[centroid[coord[x,y,z]]]]]
  vector<vector<vector<vector<vector<float>>>>> almacen;
  //   x[     y[     z[centroid[index]]]]
  vector<vector<vector<vector<int>>>> centroidIndex;
  vector<vector<vector<bool>>> cubeNotEmpty(
      counts[0], vector<vector<bool>>(counts[1], vector<bool>(counts[2])));

  almacen.resize(counts[0]);
  centroidIndex.resize(counts[0]);

  for (int ix = 0; ix < counts[0]; ix++)
  {

    almacen[ix].resize(counts[1]);
    centroidIndex[ix].resize(counts[1]);
    // cubeNotEmpty[ix] = new bool*[counts[1]];

    for (int iy = 0; iy < counts[1]; iy++)
    {

      almacen[ix][iy].resize(counts[2]);
      centroidIndex[ix][iy].resize(counts[2]);
      // cubeNotEmpty[ix][iy] = new bool[counts[2]];

      for (int iz = 0; iz < counts[2]; iz++)
      {
        cubeNotEmpty[ix][iy][iz] = false;
      }
    }
  }

  //  En cada cubo almacena una lista de centroides y su indice
  // Each cube stores a list of centroids and their index

  for (int i = 0; i < polygons.rows() * pow(4, N); i++)
  {

    int I[3];

#pragma omp simd
    for (int j = 0; j < 3; j++)
      I[j] = (centroids[i][j] - index[j][0]) / step;

    almacen[I[0]][I[1]][I[2]].emplace_back(centroids[i]);
    // centroidIndex links the centroid back to its original polygon (so
    // four centroids per polygon)
    centroidIndex[I[0]][I[1]][I[2]].emplace_back(int(i / pow(4, N)));
    cubeNotEmpty[I[0]][I[1]][I[2]] = true;
  }

  // ============================================================

  for (int i = 0; i < (int)bundles.size(); i++)
  {

    cout << "Bundle: " << i << "/" << bundles.size();
    cout << ", Num. Fibers: " << bundles[i].fibers.size() << endl;

    vector<int> listFibInd(bundles[i].fibers.size());
    vector<vector<int>> listTri(bundles[i].fibers.size(), vector<int>(2));
    vector<vector<vector<float>>> listPtInt(bundles[i].fibers.size(),
                                            vector<vector<float>>(2));

    cout << "Find intersections for bundle " << i << endl;
#pragma omp parallel for schedule(auto)
    for (int j = 0; j < (int)bundles[i].fibers.size(); j++)
    {
      bool findInt;                   // find intersection
      int InT, FnT;                   // initial and final triangle
      vector<float> InPtInt, FnPtInt; // Initial and Final point intersection

      findInt = getMeshAndFiberIntersection(
          bundles[i].fibers[j], bundles[i].fibers[j].size(), nPtsLine, N, npbp, index, step,
          cubeNotEmpty, centroidIndex, almacen, vertex, polygons, InT, FnT,
          InPtInt, FnPtInt);
      if (!findInt)
      {
        listFibInd[j] = -1;
        continue;
      }
      listFibInd[j] = j;
      listTri[j] = ((vector<int>){InT, FnT});
      listPtInt[j] =
          ((vector<vector<float>>){{InPtInt[0], InPtInt[1], InPtInt[2]},
                                   {FnPtInt[0], FnPtInt[1], FnPtInt[2]}});
    }
    for (int j = 0; j < (int)listTri.size(); j++)
    {
      if (listFibInd[j] != -1)
      {
        this->frontIntersections[i].triangles.push_back(listTri[j][0]);
        this->frontIntersections[i].points.push_back(listPtInt[j][0]);
        this->backIntersections[i].triangles.push_back(listTri[j][1]);
        this->backIntersections[i].points.push_back(listPtInt[j][1]);
      }
    }
    listFibInd.erase(remove_if(listFibInd.begin(), listFibInd.end(),
                               [](int i)
                               { return i == -1; }),
                     listFibInd.end());
    this->fibIndex[i] = listFibInd;
  }
}

const vector<map<int, int>> &CorticalIntersection::getTrianglesIntersected()
{
  if (this->trianglesIntersected.size() == 0)
  {
    // Loop through triangle indices
    for (uint32_t i = 0; i < this->mesh.polygons.size(); i++)
    {
      // For each triangle, find each bundle that intersects it, getting a count of
      // the number of intersections
      map<int, int> intersections;
      VecProxy<BundleIntersection> allIntersections(frontIntersections, backIntersections);
      for (uint32_t j = 0; j < allIntersections.size(); j++)
      {
        const vector<int> &triangles = allIntersections[j].triangles;

        uint32_t count = 0;
        for (const auto &triangle : triangles)
        {
          if (triangle == i)
          {
            count++;
          }
        }
        if (count)
        {
          intersections.insert({j, count});
        }
      }
      this->trianglesIntersected.push_back(intersections);
    }
  }
  return this->trianglesIntersected;
}
