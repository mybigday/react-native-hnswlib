import * as React from 'react';

import { StyleSheet, View, Text } from 'react-native';
import { HierarchicalNSW } from 'react-native-hnswlib';

export default function App() {
  const [result, setResult] = React.useState<object | null>(null);

  React.useEffect(() => {
    const index = new HierarchicalNSW('l2', 2);
    index.initIndex(10);
    for (let i = 0; i < 10; i++) {
      index.addPoint([i, i], i);
    }

    const searchResult = index.searchKnn([0, 0], 3);
    setResult(searchResult);
  }, []);

  return (
    <View style={styles.container}>
      <Text>Result: {JSON.stringify(result, null, 2)}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
});
