type SpaceName = 'l2' | 'ip' | 'cosine';

type SearchResult = {
  distances: number[];
  neighbors: number[];
};

type FilterFunction = (id: number) => boolean;

type HNSWOptions = {
  maxElements: number;
  m?: number;
  efConstruction?: number;
  randomSeed?: number;
  allowReplaceDeleted?: boolean;
};

declare class HierarchicalNSW {
  constructor(spaceName: SpaceName, numDimensions: number);
  initIndex(
    maxElementsOrOpts: number | HNSWOptions,
    m?: number,
    efConstruction?: number,
    randomSeed?: number,
    allowReplaceDeleted?: boolean
  ): void;
  readIndex(filename: string, allowReplaceDeleted?: boolean): Promise<boolean>;
  readIndexSync(filename: string, allowReplaceDeleted?: boolean): void;
  writeIndex(filename: string): Promise<boolean>;
  writeIndexSync(filename: string): void;
  resizeIndex(newMaxElements: number): void;
  addPoint(point: number[], label: number, replaceDeleted?: boolean): void;
  markDelete(label: number): void;
  unmarkDelete(label: number): void;
  searchKnn(
    queryPoint: number[],
    numNeighbors: number,
    filter?: FilterFunction
  ): SearchResult;
  getIdsList(): number[];
  getPoint(label: number): number[];
  getMaxElements(): number;
  getCurrentCount(): number;
  getNumDimensions(): number;
  getEf(): number;
  setEf(ef: number): void;
}

declare class InnerProductSpace {
  constructor(numDimensions: number);
  distance(pointA: number[], pointB: number[]): number;
  getNumDimensions(): number;
}

declare class L2Space {
  constructor(numDimensions: number);
  distance(pointA: number[], pointB: number[]): number;
  getNumDimensions(): number;
}

declare class BruteforceSearch {
  constructor(space: string, dimension: number, index: number[], ids: number[]);
  search(query: number[], k: number, filter?: FilterFunction): SearchResult;
  addItem(item: number[], id: number): void;
  removeItem(id: number): void;
  saveIndex(): void;
  loadIndex(): void;
}

declare interface HnswlibImpl {
  HierarchicalNSW: typeof HierarchicalNSW;
  InnerProductSpace: typeof InnerProductSpace;
  L2Space: typeof L2Space;
  BruteforceSearch: typeof BruteforceSearch;
}

declare const hnswlib: HnswlibImpl;
