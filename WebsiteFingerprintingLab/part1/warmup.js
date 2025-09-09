const runs = 10;

function measureOneLine() {
  const LINE_SIZE = 16; // 64/sizeof(int)
  let result = [];

  // Fill with -1 to ensure allocation
  const M = new Array(runs * LINE_SIZE).fill(-1);

  for (let i = 0; i < runs; i++) {
    const start = performance.now();
    let val = M[i * LINE_SIZE];
    const end = performance.now();

    result.push(end - start);
  }

  return result;
}

function measureNLines(N) {
  const LINE_SIZE = 16; // 64/sizeof(int)
  let result = [];
  let max_lines = 1000000;

  // Fill with -1 to ensure allocation
  const M = new Array(LINE_SIZE * max_lines).fill(-1);


  for (let i = 0; i < runs; i++) {
    const start = performance.now();
    for (let j = 0; j < N; j++) {
      let val = M[(j * LINE_SIZE) % (LINE_SIZE * max_lines)];
    }
    const end = performance.now();

    result.push(end - start);
  }

  console.log(median(result));

  return result;
}

function median(a){ a.sort((x,y)=>x-y); return a[a.length>>1]; }

document.getElementById(
  "exercise1-values"
).innerText = `1 Cache Line: [${measureOneLine().join(", ")}]`;
for (let i = 1; i < 100000000; i *= 10){
  document.getElementById(
  "exercise2-values"
  ).innerText = `N Cache Lines: [${measureNLines(i).join(", ")}]`;
}