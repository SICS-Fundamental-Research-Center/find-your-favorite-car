// read points from a text file
export function parsePoints(text) {
  const points = text
    .trim()
    .split("\n")
    .slice(1) // the first line is <numOfPoints> <dimension>
    .map(line =>
      line
        .trim()
        .split(/\s+/)
        .map(str => parseFloat(str))
    );
  return points;
}

// read labels from a text file
export function parseLabels(text) {
  const labels = text.trim().split("\n");
  return labels;
}

// check if val is in the range [low, high].
function isInRange(val, low, high) {
  return low <= val && val <= high;
}

// get the range of each attribute in a set of points.
export const getRanges = points => {
  const ranges = points[0].map(x => ({ low: x, high: x }));
  points.slice(1).forEach(point => {
    point.forEach((x, i) => {
      ranges[i].low = Math.min(ranges[i].low, x);
      ranges[i].high = Math.max(ranges[i].high, x);
    });
  });
  return ranges;
};

// load a dataset by reading its points and labels.
export const loadDataset = async (pointsURL, labelsURL) => {
  let response = await fetch(pointsURL);
  const points = parsePoints(await response.text());
  if (labelsURL === undefined) {
    return points;
  }
  response = await fetch(labelsURL);
  const labels = parseLabels(await response.text());
  return [points, labels];
};

// get points that are in the specified ranges.
export const selectCandidates = (points, ranges, mask, maxPoints) => {
  const candidates = [];
  for (let i = 0; i < points.length; ++i) {
    if (candidates.length >= maxPoints) break;
    const point = points[i];
    let isValid = true;
    for (let j = 0; j < point.length; ++j) {
      if (mask[j] && !isInRange(point[j], ranges[j][0], ranges[j][1])) {
        isValid = false;
        break;
      }
    }
    if (isValid) candidates.push(point);
  }
  return candidates;
};

// convert a JS array to a C++ 2D vector
export const array2Vector2D = array => {
  const vector = new window.Module.VecVecDouble();
  array.forEach(arr => {
    const vec = new window.Module.VectorDouble();
    arr.forEach(x => vec.push_back(x));
    vector.push_back(vec);
    vec.delete();
  });
  return vector;
};

// convert a C++ 2D vector to a JS array
export const vector2Array2D = vector => {
  const array = [];
  for (let i = 0; i < vector.size(); ++i) {
    const vec = vector.get(i);
    const arr = [];
    for (let j = 0; j < vec.size(); ++j) arr.push(vec.get(j));
    array.push(arr);
  }
  return array;
};

// convert a C++ vector to a JS array
export const vector2Array = vector => {
  const array = [];
  for (let i = 0; i < vector.size(); ++i) {
    array.push(vector.get(i));
  }
  return array;
};

// convert a JS array to a C++ vector
export const array2Vector = array => {
  const vec = new window.Module.VectorDouble();
  array.forEach(x => vec.push_back(x));
  return vec;
};



// get the indices of points pruned.
// both prevIndices and currIndices need to be sorted.
export const getPrunedIndices = (prevIndices, currIndices) => {
  let prunedIndices = [];
  for (let i = 0, j = 0; i < prevIndices.size() || j < currIndices.size();) {
    if (j >= currIndices.size() || prevIndices.get(i) < currIndices.get(j)) {
      prunedIndices.push(prevIndices.get(i));
      ++i;
    } else {
      ++i;
      ++j;
    }
  }
  return prunedIndices;
};

const dominates = (p1, p2, smallerBetter, isSelected) => {
  let i;
  for (i = 0; i < p1.length; i++) {
    // console.log('121313', isSelected[i], p1.length);
    // if (isSelected) {
    //   console.log('121313', isSelected, smallerBetter);
    // }
    // if (isSelected == undefined) {
    //   console.log('dwadadw');
    // }
    if (isSelected.at(i) == 1) {
      // if ( isSelected.at(i) == 1) {
      // console.log('121313', isSelected[i], smallerBetter[i]);
      if (smallerBetter[i] == 1) {
        if (p1[i] > p2[i]) return 0;
      } else {
        if (p1[i] < p2[i]) return 0;
      }
    }
  }
  return 1;
}

export const getSkyline = (points, smallerBetter, isSelected) => {
  let i, j, dominated, index = 0, m;
  console.log(smallerBetter, isSelected);
  let sl = new Array(points.length);
  for (i = 0; i < points.length; i++) {

    dominated = 0;
    const pt = points.at(i).slice();

    for (j = 0; j < index && !dominated; ++j) {
      if (dominates(points[sl[j]], pt, smallerBetter, isSelected))
        dominated = 1;
    }

    if (!dominated) {
      m = index;
      index = 0;
      for (j = 0; j < m; j++) {
        if (!dominates(pt, points[sl[j]], smallerBetter, isSelected)) {
          sl[index++] = sl[j];
        }
      }
      sl[index++] = i;
    }
  }
  return index;
}

export const dist = (p, q) => {
  let res = 0;
  for (let i = 0; i < p.length; i++) {
    res += Math.abs(p[i] = q[i]);
  }
  return res;
}

export const getMaxDist = (vertices) => {
  console.log('getMaxDist', vertices);
  let maxDist = 0;
  for (let i = 0; i < vertices.length; i++) {
    for (let j = 0; j < vertices.length; j++) {
      let p = vertices.at(i);
      let q = vertices.at(j);
      if (dist(p, q) > maxDist) {
        maxDist = dist(p, q);
      }
    }
  }
  console.log('getMaxDist res', maxDist);
  return maxDist;
}