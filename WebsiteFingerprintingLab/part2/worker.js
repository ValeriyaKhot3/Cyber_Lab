//Interval length in ms
let P = 1000;

// Duration of your trace, in milliseconds
let TRACE_LENGTH;

// Array of length TRACE_LENGTH with your trace's values
let T;

// Value of performance.now() when you started recording your trace
let start = 0;

// the interval time fron start
let time =0 ;

// Num of entire cache reading
let cycles = 0;

//traces interval indx
let P_indx = 0;

//L3 cache size
let cache_size = 12*(2**20);

//L3 buffer
let cache;

//Line cache size
let line_size = 64;

function record() {
  P_indx = 0;
  start = 0;
  time =0 ;
  cycles = 0;

  // Create empty array for saving trace values
  T = new Array(TRACE_LENGTH);
  cache = new Uint8Array(cache_size);

  // Fill array with -1 so we can be sure memory is allocated
  T.fill(-1, 0, T.length);

  // Save start timestamp
  start = performance.now();

  // TODO (Exercise 2-2): Record data for TRACE_LENGTH seconds and save values to T.
  console.log("start tracing");
  while(P_indx<T.length){
    cycles_count();
  }
  console.log("end tracing");
    

  // Once done recording, send result to main thread
  postMessage(JSON.stringify(T));
}

// DO NOT MODIFY BELOW THIS LINE -- PROVIDED BY COURSE STAFF
self.onmessage = (e) => {
  if (e.data.type === "start") {
    TRACE_LENGTH = 100;
    //TRACE_LENGTH = e.data.trace_length;
    setTimeout(record, 0);
  }
};

function cycles_count(){
  for(let i = 0; i< cache.length; i+=line_size){
    let val = cache[i];
    time = performance.now() - start;
    if(time >= P){
      start = performance.now();
      T[P_indx++] = cycles;
      cycles = 0;
      break;
    }
    else if(i == cache.length-line_size){
      cycles++;
    }
  }

}