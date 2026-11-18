/*
Available macros:
    NUM_VOLUMES:      The number of input volumes, corresponds to the dimensionality of the attribute space
    TRAIT_ALLOCATION: Maximum number of traits
*/

/*
* Calculates the euclidean distance of an n-dimensional point to an n-dimensional range trait.
*/
float euclideanDistanceToRangeTrait(float point[NUM_VOLUMES], mat4 trait) {
  /*
  float dist = 0.0f;

  for(int i = 0; i < NUM_VOLUMES; i++) {
    float diff = point[i] - trait[i];
    dist += diff * diff;
  }

#ifdef SQUARED_DISTANCE
  return dist;
#else
  return sqrt(dist);
#endif
*/
  return 0;
}

float minDistanceToSetOfRangeTraits(float point[NUM_VOLUMES], mat4 traits[TRAIT_ALLOCATION], int numPointTraits) {
  float minDistance = 1.0f / 0.0f;

  for (int i = 0; i < numPointTraits; i++) {
    mat4 trait = traits[i];
    minDistance = min(euclideanDistanceToRangeTrait(point, trait), minDistance);
  }

  return minDistance;
}
