# Lossless Compression Algorithms Testing : 
Playing around with the most common lossless compression algorithms. Was interested in how they perform for our use cases, so tried profiling the performance of the algorithms to get real-world insights.

Rough test results and insights mentioned below. Structured results present in our blog.

## Custom Deflate Output validated with zlib deflate decompressor - Fixed Huffman Coding
<img width="654" height="101" alt="image" src="https://github.com/user-attachments/assets/5dac92ce-ab06-48e8-9269-190524e1e13a" />


# Custom Implementation
## Huffman Encoding
<img width="1151" height="382" alt="image" src="https://github.com/user-attachments/assets/a45305b8-415f-438c-8c23-7164574a5032" />

## LZ77 Compression
### Expansion of compressed data size
<img width="1680" height="604" alt="image" src="https://github.com/user-attachments/assets/96bc8c2e-d13f-4328-950a-8f59e27b7581" />

### No Expansion of compressed data size
<img width="1700" height="241" alt="image" src="https://github.com/user-attachments/assets/318c911f-bfa6-40f6-8270-7a340a79017e" />

## DEFLATE
<img width="1238" height="106" alt="image" src="https://github.com/user-attachments/assets/d7343bda-ebd7-4b05-a60d-4f15b29e64af" />

- I see an expansion with the Huffman after LZ77 Compression here. From preliminary read ups, I see, one would need to use Bit Packed Fix for this.

### With Bit Packed Encoding
<img width="847" height="117" alt="image" src="https://github.com/user-attachments/assets/ae0c9277-cea4-45f8-a706-547c7b450302" />

- Decompression verified --> 4KB Sliding Window Size, 258 Bytes - Look Ahead Buffer Size.

- As per https://patents.google.com/patent/US9577665B2/en - Sliding Window Size is supposed to be 32KB. 

<img width="876" height="116" alt="image" src="https://github.com/user-attachments/assets/dc3127a4-cafb-4e08-87d4-0c1edb0750da" />

- Decompression verified, and Compression Ratio improved. Longer context helps.


### Difference in size between normal and bitpacked
<img width="330" height="42" alt="image" src="https://github.com/user-attachments/assets/1a3ec298-ed81-4eaa-8d1e-77eb437e8b37" />

- Non Bit Packed :: 231104
- Bit Packed :: 28888
- Non Bit Packed / Bit Packed = 8 as expected.
- Normal string "10101", each 1 or 0 is stored into a Byte(8 bits). Bit packing stores 8 such bits into 1 Byte.  


# Form Model JSON Performance Metrics
## Brotli Grid search on tunable parameters for Form Model JSON : 

| Optimized For | Quality | Mode | Lgwin | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|-------------------|-------------------|
| Optimized Time | 0 | 0 | 23 |   91.10% |   0.001415s |     0.003537 MB |  11.2349
| Optimized Peak Memory | 3 | 2 | 10 |   81.56% |   0.003139s |     0.002314 MB |   5.4241
| Optimized Compression Percentage | 11 | 2 | 19 |   95.60% |   0.339854s |     0.002605 MB |  22.7455

- Higher quality : Higher Compression Percentage : Slower Compression
- Lower Quality : Lower compression percentage : Faster compression
- Mode : 0 : Generic :: Treats input as arbitrary data.
- Mode : 1 : Text :: Optimized for UTF-8
- Mode : 2 : Font :: Optimized for WOFF/WOFF2 fonts
- Lgwin : Size of sliding window
- Lower Lgwin : Smaller Sliding window of dictionary : Faster compression : Lower Compression Percentage
- Higher Lgwin : Larger Sliding window of dictionary : Slower compression : Higher Compression Percentage


## GZip Search on Tunable Parameter for Form Model JSON : 
| Optimized For | Compress Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|----------------|------------------------|------------|-------------------|-------------------|
| Optimized Time | 3 |   90.50% |   0.003147s |     0.007225 MB |  10.5318
| Optimized Peak Memory | 9 |   92.96% |   0.012399s |     0.006429 MB |  14.2012
| Optimized Compression Percentage | 9 |   92.96% |   0.012399s |     0.006429 MB |  14.2012

## ZSTD Search on Tunable Parameter for Form Model JSON : 
| Optimized For | Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|
| Optimized Time | 3 |   92.84% |   0.001979s |     0.004005 MB |  13.9645
| Optimized Peak Memory | 20 |   95.28% |   0.057491s |     0.002149 MB |  21.1681
| Optimized Compression Percentage | 22 |   95.32% |   0.125617s |     0.002250 MB |  21.3697

## LZ4 Grid Search on Tunable Parameters for Form Model JSON : 
| Optimized For | Compression Level | Block Size | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|-------------------|
| Optimized Time | 2 | 5 |   88.38% |   0.001291s |     0.002087 MB |   8.6065
| Optimized Peak Memory | 2 | 7 |   88.39% |   0.001332s |     0.001910 MB |   8.6150
| Optimized Compression Percentage | 12 | 6 |   92.79% |   0.013155s |     0.001962 MB |  13.8697

## Snappy Does not have Tunable Parameters
| Optimized For | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|
| Snappy |   86.02% | 0.002482s | 0.005855 MB |   7.1522


## Form Model JSON Performance
| Optimized For |  Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|------------------------|------------|-------------------|-------------------|
| Brotli - Optimized Time |   91.10% |   0.001415s |     0.003537 MB |  11.2349
| Brotli - Optimized Peak Memory |   81.56% |   0.003139s |     0.002314 MB |   5.4241
| Brotli - Optimized Compression Percentage |   95.60% |   0.339854s |     0.002605 MB |  22.7455
| GZip - Optimized Time |   90.50% |   0.003147s |     0.007225 MB |  10.5318
| GZip - Optimized Peak Memory |   92.96% |   0.012399s |     0.006429 MB |  14.2012
| GZip - Optimized Compression Percentage |   92.96% |   0.012399s |     0.006429 MB |  14.2012
| ZSTD - Optimized Time |   92.84% |   0.001979s |     0.004005 MB |  13.9645
| ZSTD - Optimized Peak Memory |   95.28% |   0.057491s |     0.002149 MB |  21.1681
| ZSTD - Optimized Compression Percentage |   95.32% |   0.125617s |     0.002250 MB |  21.3697
| LZ4 - Optimized Time |   88.38% |   0.001291s |     0.002087 MB |   8.6065
| LZ4 - Optimized Peak Memory |   88.39% |   0.001332s |     0.001910 MB |   8.6150
| LZ4 - Optimized Compression Percentage |   92.79% |   0.013155s |     0.001962 MB |  13.8697
| Snappy - Optimized |   86.02% | 0.002113s | 0.005807 MB |  86.0183

##################################################################
| Direct Huffman | 57.1196% | x | x |  2.33207
| LZ77 Compression | 86.7016% | x | x |  7.51973
| Custom Deflate Impl | 89.3233% | x | x |  9.3662

# HTML Performance Metrics
## Brotli Grid search on tunable parameters for HTML : 
| Optimized For | Quality | Mode | Lgwin | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|-------------------|-------------------|
| Optimized Time | 0 | 0 | 24 |   86.83% |   0.003225s |     0.003514 MB |   7.5909
| Optimized Peak Memory | 6 | 0 | 13 |   88.15% |   0.007674s |     0.002314 MB |   8.4416
| Optimized Compression Percentage | 11 | 0 | 20 |   92.60% |   0.717705s |     0.002383 MB |  13.5059

## GZip Grid Search on Tunable Parameters for HTML : 
| Optimized For | Compress Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|----------------|------------------------|------------|-------------------|-------------------|
| Optimized Time | 3 |   87.11% |   0.006509s |     0.007180 MB |   7.7586
| Optimized Peak Memory | 9 |   89.20% |   0.017365s |     0.006388 MB |   9.2613
| Optimized Compression Percentage | 9 |   89.20% |   0.017365s |     0.006388 MB |   9.2613

## ZSTD Grid Search on Tunable Parameters for HTML : 
| Optimized For | Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|
| Optimized Time | 3 |   89.74% |   0.003602s |     0.003957 MB |   9.7459
| Optimized Peak Memory | 22 |   92.07% |   0.206914s |     0.002134 MB |  12.6126
| Optimized Compression Percentage | 22 |   92.07% |   0.206914s |     0.002134 MB |  12.6126

## LZ4 Grid Search on Tunable Parameters for HTML : 
| Optimized For | Compression Level | Block Size | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|-----------|--------|
| Optimized Time | 2 | 5 |   82.75% |   0.002593s |     0.001970 MB |   5.7966
| Optimized Peak Memory | 7 | 4 |   87.70% |   0.006764s |     0.001920 MB |   8.1334
| Optimized Compression Percentage | 12 | 6 |   87.84% |   0.023204s |     0.002087 MB |   8.2254

## Snappy Performance for HTML : 
| Optimized For | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|
| Optimized Snappy |   79.56% | 0.005046s | 0.005855 MB |   4.8915

## HTML Compiled Performance
| Optimized For |  Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|------------------------|------------|-------------------|-------------------|
| Brotli - Optimized Time |   86.83% |   0.003225s |     0.003514 MB |   7.5909
| Brotli - Optimized Peak Memory |   88.15% |   0.007674s |     0.002314 MB |   8.4416
| Brotli - Optimized Compression Percentage |   92.60% |   0.717705s |     0.002383 MB |  13.5059
| GZip - Optimized Time |   87.11% |   0.006509s |     0.007180 MB |   7.7586
| GZip - Optimized Peak Memory |   89.20% |   0.017365s |     0.006388 MB |   9.2613
| GZip - Optimized Compression Percentage |   89.20% |   0.017365s |     0.006388 MB |   9.2613
| ZSTD - Optimized Time |   89.74% |   0.003602s |     0.003957 MB |   9.7459
| ZSTD - Optimized Peak Memory |   92.07% |   0.206914s |     0.002134 MB |  12.6126
| ZSTD - Optimized Compression Percentage |   92.07% |   0.206914s |     0.002134 MB |  12.6126
| LZ4 - Optimized Time |   82.75% |   0.002593s |     0.001970 MB |   5.7966
| LZ4 - Optimized Peak Memory |   87.70% |   0.006764s |     0.001920 MB |   8.1334
| LZ4 - Optimized Compression Percentage |   87.84% |   0.023204s |     0.002087 MB |   8.2254
| Snappy |   79.56% | 0.005046s | 0.005855 MB |   4.8915

##################################################################
| Direct Huffman | 34.729% | x | x |  1.53207
| LZ77 Compression | 79.1667% | x | x |  4.8
| Custom Deflate Impl | 83.4046% | x | x |  6.02575

# JS Performance Metrics
## Brotli Grid search on tunable parameters for JS : 
| Optimized For | Quality | Mode | Lgwin | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|-------------------|-------------------|
| Optimized Time | 0 | 0 | 18 |   76.46% |   0.006546s |     0.003686 MB |   4.2474
| Optimized Peak Memory | 5 | 1 | 23 |   83.15% |   0.017284s |     0.002270 MB |   5.9355
| Optimized Compression Percentage | 11 | 0 | 21 |   85.63% |   1.269449s |     0.002322 MB |   6.9584

## GZip Grid Search on Tunable Parameters for JS : 
| Optimized For | Compress Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|----------------|------------------------|------------|-------------------|-------------------|
| Optimized Time | 2 |   77.45% |   0.012263s |     0.007233 MB |   4.4348
| Optimized Peak Memory | 8 |   81.66% |   0.045796s |     0.006390 MB |   5.4515
| Optimized Compression Percentage | 9 |   81.69% |   0.080125s |     0.006494 MB |   5.4602

## ZSTD Grid Search on Tunable Parameters for JS : 
| Optimized For | Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|
| Optimized Time | 4 |   80.20% |   0.007290s |     0.003898 MB |   5.0515
| Optimized Peak Memory | 20 |   85.12% |   0.207782s |     0.002201 MB |   6.7185
| Optimized Compression Percentage | 22 |   85.13% |   0.228938s |     0.002301 MB |   6.7253

## LZ4 Grid Search on Tunable Parameters for JS : 
| Optimized For | Compression Level | Block Size | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|-------------------|
| Optimized Time | -3 | 4 |   65.37% |   0.004630s |     0.003145 MB |   2.8874
| Optimized Peak Memory | 0 | 6 |   69.08% |   0.004874s |     0.002019 MB |   3.2343
| Optimized Compression Percentage | 12 | 7 |   78.36% |   0.089520s |     0.002020 MB |   4.6205

## Snappy Performance for JS : 
| Optimized For | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|
| Optimized Snappy |   69.18% | 0.007227s | 0.005855 MB |   3.2447

## JS Compiled Performance
| Optimized For |  Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|------------------------|------------|-------------------|-------------------|
| Brotli - Optimized Time |   76.46% |   0.006546s |     0.003686 MB |   4.2474
| Brotli - Optimized Peak Memory |   83.15% |   0.017284s |     0.002270 MB |   5.9355
| Brotli - Optimized Compression Percentage |   85.63% |   1.269449s |     0.002322 MB |   6.9584
| GZip - Optimized Time | 77.45% |   0.012263s |     0.007233 MB |   4.4348
| GZip - Optimized Peak Memory | 81.66% |   0.045796s |     0.006390 MB |   5.4515
| GZip - Optimized Compression Percentage |   81.69% |   0.080125s |     0.006494 MB |   5.4602
| ZSTD - Optimized Time |   80.20% |   0.007290s |     0.003898 MB |   5.0515
| ZSTD - Optimized Peak Memory |   85.12% |   0.207782s |     0.002201 MB |   6.7185
| ZSTD - Optimized Compression Percentage |   85.13% |   0.228938s |     0.002301 MB |   6.7253
| LZ4 - Optimized Time |   65.37% |   0.004630s |     0.003145 MB |   2.8874
| LZ4 - Optimized Peak Memory |   69.08% |   0.004874s |     0.002019 MB |   3.2343
| LZ4 - Optimized Compression Percentage |   78.36% |   0.089520s |     0.002020 MB |   4.6205
| Snappy |   69.18% | 0.007227s | 0.005855 MB |   3.2447

##################################################################
| Direct Huffman | 44.7337% | x | x |  1.80942
| LZ77 Compression | 64.5201% | x | x |  2.8185
| Custom Deflate Impl | 71.642% | x | x |  3.52634


# CSS Performance Metrics
## Brotli Grid search on tunable parameters for CSS : 
| Optimized For | Quality | Mode | Lgwin | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|-------------------|-------------------|
| Optimized Time | 0 | 1 | 23 |   87.06% |   0.000601s |     0.002516 MB |   7.7306
| Optimized Peak Memory | 1 | 2 | 16 |   86.11% |   0.000904s |     0.002356 MB |   7.1990
| Optimized Compression Percentage | 11 | 0 | 18 |   93.16% |   0.152097s |     0.002374 MB |  14.6207

## GZip Grid Search on Tunable Parameters for CSS : 
| Optimized For | Compress Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|----------------|------------------------|------------|-------------------|-------------------|
| Optimized Time | 3 |   88.18% |   0.001475s |     0.007225 MB |   8.4622
| Optimized Peak Memory | 9 |   90.82% |   0.004837s |     0.006275 MB |  10.8945
| Optimized Compression Percentage | 9 |   90.82% |   0.004837s |     0.006275 MB |  10.8945

## ZSTD Grid Search on Tunable Parameters for CSS : 
| Optimized For | Level | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|
| Optimized Time | 2 |   89.30% |   0.000961s |     0.003965 MB |   9.3452
| Optimized Peak Memory | 22 |   92.80% |   0.052090s |     0.002134 MB |  13.8803
| Optimized Compression Percentage | 22 |   92.80% |   0.052090s |     0.002134 MB |  13.8803

## LZ4 Grid Search on Tunable Parameters for CSS : 
| Optimized For | Compression Level | Block Size | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|------------|-------------------|
| Optimized Time | 2 | 4 |   82.99% |   0.000646s |     0.002369 MB |   5.8778
| Optimized Peak Memory | 11 | 0 |   89.57% |   0.004444s |     0.001910 MB |   9.5907
| Optimized Compression Percentage | 12 | 5 |   89.62% |   0.005314s |     0.002369 MB |   9.6346

## Snappy Performance for CSS : 
| Optimized For | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|---------|------|-------|------------------------|
| Optimized Snappy |   81.12% | 0.004165s | 0.005744 MB |   5.2957

## CSS Compiled Performance
| Optimized For |  Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|--------------|------------------------|------------|-------------------|-------------------|
| Brotli - Optimized Time |   87.06% |   0.000601s |     0.002516 MB |   7.7306
| Brotli - Optimized Peak Memory |   86.11% |   0.000904s |     0.002356 MB |   7.1990
| Brotli - Optimized Compression Percentage |   93.16% |   0.152097s |     0.002374 MB |  14.6207
| GZip - Optimized Time |   88.18% |   0.001475s |     0.007225 MB |   8.4622
| GZip - Optimized Peak Memory |   90.82% |   0.004837s |     0.006275 MB |  10.8945
| GZip - Optimized Compression Percentage |   90.82% |   0.004837s |     0.006275 MB |  10.8945
| ZSTD - Optimized Time |   89.30% |   0.000961s |     0.003965 MB |   9.3452
| ZSTD - Optimized Peak Memory |   92.80% |   0.052090s |     0.002134 MB |  13.8803
| ZSTD - Optimized Compression Percentage |   92.80% |   0.052090s |     0.002134 MB |  13.8803
| LZ4 - Optimized Time |   82.99% |   0.000646s |     0.002369 MB |   5.8778
| LZ4 - Optimized Peak Memory |   89.57% |   0.004444s |     0.001910 MB |   9.5907
| LZ4 - Optimized Compression Percentage |   89.62% |   0.005314s |     0.002369 MB |   9.6346
| Snappy |   81.12% | 0.004165s | 0.005744 MB |   5.2957

##################################################################
| Direct Huffman | 36.4418% | x | x |  1.57336
| LZ77 Compression | 82.4449% | x | x |  5.69636
| Custom Deflate Impl | 85.96% | x | x | 7.1225


## Brotli Tunable parameters and what their ranges mean : 
- quality (0–11)
Meaning:

Determines the compression effort vs speed trade-off.

Higher quality → algorithm searches more for repeated patterns → smaller files → slower compression.
Lower quality → faster compression, less optimal size reduction.
Range: 0–11

Quality	Behavior
0–4	Very fast, low compression. Good for streaming or low-latency applications.
5–8	Balanced — decent compression without extreme compute cost. Often used in CDNs.
9–11	Maximum compression — best for build-time/pre-compression. Slower but reduces file size the most.

Theoretical impact:
Controls how deeply the algorithm searches for redundancies.
Think of it as “how hard should Brotli work to shrink the file?”

- mode (0–2)
Meaning:
Gives Brotli a hint about the type of data to optimize compression strategy.
There are only three modes:
- Mode	Value	Theoretical meaning
- Generic	0	Default. Treats input as arbitrary data, balances compression heuristics.
- Text	1	Optimized for UTF-8 text. Looks for language patterns, spaces, repeated words.
- Font	2	Optimized for WOFF/WOFF2 fonts. Handles repeated font glyph patterns efficiently.

Theoretical impact:
Adjusts pattern search heuristics and probability models based on data type.
Using the right mode = slightly better compression and faster processing.

- lgwin (10–24)
Meaning:
Sets the size of the sliding window (dynamic dictionary).

Brotli stores the most recent 2^lgwin bytes in memory to reference repeated patterns.

Range: 10–24 → window = 2^lgwin bytes

lgwin	Window size	Use case / impact
10	1 KB	Tiny window. Good for small files, fast, but misses long-range redundancy.
16	64 KB	Similar to Gzip default. Medium compression for moderately sized files.
19	512 KB	Large enough for typical JS bundles or HTML templates.
22	4 MB	Large files, highly repetitive. Can detect far-apart patterns → better compression.
24	16 MB	Maximum window size. Best for huge repetitive content. Slower, more memory needed.

Theoretical impact:
Larger window → more long-range pattern detection → higher compression ratio.
Smaller window → less memory, faster compression, might miss patterns.


## Lossless compression test with entire Form JSON
### Code : 
<img width="861" height="465" alt="image" src="https://github.com/user-attachments/assets/f0ecef56-0a83-4baf-b39d-23642e6e13c4" />

### Output for FORM Model JSON : 
<img width="393" height="75" alt="image" src="https://github.com/user-attachments/assets/aeb61376-f518-4149-8623-59c6c1df7f2b" />


Damn, that's a 95% reduction in size. 0.5mb worth of json stringified is compressed to basically 24kb

Apparently it's not as good for Digital Media compression as PNG, JPG etc.

But mainly practical use cases are : 

### Practical Uses
- Websites: Serve HTML/CSS/JS/JSON compressed
- APIs: Compress JSON payloads for faster network transfer
- CDNs: Edge compression before serving files
- Local storage: Save text-heavy logs or configs in a compressed form



### Code for HTML Compression : 
<img width="864" height="791" alt="image" src="https://github.com/user-attachments/assets/8c20786a-23ca-427d-8818-f8198e7e5e7a" />

### Output for HTML Compression on an entire form html
<img width="419" height="61" alt="image" src="https://github.com/user-attachments/assets/67101df0-6ec4-44af-8f3a-c09f3aa1bcfd" />
Again 92% compression. Almost 1mb to 75 kb.


Interesting : 
<img width="879" height="639" alt="image" src="https://github.com/user-attachments/assets/c8670bdd-3b56-4d2b-aab1-959fb321db41" />


- Brotli is very good for Static files compression. Higher CPU utilization as compared to Gzip.
- Gzip is standard for API compression. Lower CPU Utilization, Super fast. And Standardized.
- zstd is much better and being used by modern browsers, but not standardized yet.

Decompression is taken care by the browser.
Request header will have something like : ----> accept-encoding : gzip, deflate, br, zstd || which says the browser will automatically decompress all of these.
Response header will have the compression used by the server : ----> content-encoding : br || which means it used brotli.
Sometimes it'll also tell you the Content Size after decompression ---> content-length : 124


Additional Facts on other Compression Algorithms : 
  - LZ4 and Snappy are two compression algorithms that have fast software implementations with low latency but generate a low compression ratio.
  - Zstd which can offer a higher/configurable compression ratio at a lower speed, and GZIP which, based on its configuration with or without Dynamic Huffman tables, can offer high/moderate compression ratios at a moderate speed.
    <img width="754" height="255" alt="image" src="https://github.com/user-attachments/assets/57739ea1-43f1-4831-b700-9e19a53a32c4" />
    --------------------------------------------------------------------------------------------------
  - | Algorithm    | Compression Ratio | Latency  | Speed    | Use Case                             |
    | ------------ | ----------------- | -------- | -------- | ------------------------------------ |
    | Dynamic Gzip | High              | High     | Moderate | APIs, dynamic JSON                   |
    | Static Gzip  | Moderate          | Moderate | Moderate | Static assets                        |
    | LZ4 / Snappy | Low               | Low      | High     | Fast API payloads, real-time logging |
    | Zstd         | High              | High     | Moderate | Storage-heavy APIs                   |

    Latency here meaning Server side computation.

    ### Dynamic history is different for each algorithm : 
    <img width="848" height="593" alt="image" src="https://github.com/user-attachments/assets/84918f41-2201-4755-88c0-c1da7a1351b5" />
    ### When history is kept constant ->
    <img width="750" height="432" alt="image" src="https://github.com/user-attachments/assets/d7eaddb0-27fb-414c-9307-06ca22bd1bbd" />

    ### Compression Ratio / Dynamic History
    <img width="891" height="472" alt="image" src="https://github.com/user-attachments/assets/2f7713b8-1eb2-4afc-8beb-627996437cfa" />


1️⃣ Compression Ratio

Definition:
Compression ratio = (Original size) / (Compressed size)
Higher ratio → smaller compressed data relative to original → better compression efficiency.

Implication:
Yes, higher is better if your goal is to minimize storage or bandwidth.

2️⃣ History Window Size (in algorithms like LZ77, Brotli, etc.)

Definition:
The history window (or sliding window) is the amount of previously seen data the compressor can reference when encoding repeated sequences.

Effect of Larger Window:

Can reference longer matches, which often improves compression ratio.

Uses more memory, since the compressor must store the window in memory.

Effect of Smaller Window:

Uses less memory → better for low-memory environments.

Compression ratio usually drops because repeated patterns outside the small window are not detected.

3️⃣ Trade-offs

High compression ratio + small window → hard to achieve. You usually need a bigger window to find repeated sequences that help compression.

High compression ratio + large window → yes, ideal for storage/bandwidth, but memory usage increases.

Small window + speed → sometimes acceptable if memory is limited and speed is critical, even if compression ratio suffers.


| Metric              | Higher    | Better?           | Notes                                                                               |
| ------------------- | --------- | ----------------- | ----------------------------------------------------------------------------------- |
| Compression Ratio   | Yes       | More space saving | Larger history window helps improve this                                            |
| History Window Size | Sometimes | Depends           | Bigger → better compression, more memory; smaller → lower memory, lower compression |


- Higher compression ratio is always better, but higher history window is only “better” if memory usage is acceptable. If your goal is high compression with low memory


## Performance measuring on our Form Model JSON
### BROTLI
<img width="1175" height="338" alt="image" src="https://github.com/user-attachments/assets/4907502a-951d-4e58-a7cd-91a0bc1f2e92" />

### GZIP
<img width="1216" height="367" alt="image" src="https://github.com/user-attachments/assets/d9962b16-5e96-4c9a-938b-499920a72da4" />

### LZ4
<img width="1035" height="318" alt="image" src="https://github.com/user-attachments/assets/bf167006-9ba5-4094-9c2c-98ca303cb464" />

### ZSTD
<img width="1140" height="317" alt="image" src="https://github.com/user-attachments/assets/844abfab-80c8-4fab-b1af-f988b5cb1e08" />

### Snappy
<img width="1176" height="349" alt="image" src="https://github.com/user-attachments/assets/4f13e916-9b93-43f2-b5ac-8a8960fb2a0b" />

### INSIGHTS OF FORM JSON
Form Model JSON Size : 0.565331 MB
| Algorithm | Percentage of compression | Time Taken         | Current Memory Usage | Peak Memory Usage   |
|-----------|---------------------------|--------------------|----------------------|---------------------|
| Brotli    | 95.605%                   | 0.325558 seconds   | 0.005180 MB          | 2.2906 MB           |
| GZip      | 92.961%                   | 0.013879 seconds   | 0.009473 MB          | 2.3099 MB           |
| LZ4       | 88.350%                   | 0.004017 seconds   | 0.005479 MB          | 2.3313 MB           |
| ZSTD      | 92.8426%                  | 0.005683 seconds   | 0.004669 MB          | 2.83324 MB          |
| Snappy    | 86.02075%                 | 0.004010 seconds   | 0.005804 MB          | 2.3453 MB           |



## Performance measuring on a native Form HTML
### BROTLI
<img width="1113" height="367" alt="image" src="https://github.com/user-attachments/assets/56828d94-0196-47f2-a70d-d68ba9d7a649" />

### GZIP
<img width="1156" height="407" alt="image" src="https://github.com/user-attachments/assets/461890f8-3542-4c0c-abda-f40db518d242" />

### LZ4
<img width="1045" height="312" alt="image" src="https://github.com/user-attachments/assets/755bf14f-5ec0-4be0-811e-7302504a39c2" />

### ZSTD
<img width="1055" height="313" alt="image" src="https://github.com/user-attachments/assets/cf1d4b8a-3b81-4f9b-91f6-8893f4bc2c80" />

### Snappy
<img width="579" height="176" alt="image" src="https://github.com/user-attachments/assets/0e3f0a59-3014-4f79-8145-3c3e3ebfc249" />

### INSIGHTS OF HTML
HTML Size : 1.01218 MB
| Algorithm | Percentage of compression | Time Taken         | Current Memory Usage | Peak Memory Usage |
|-----------|---------------------------|--------------------|--------------------- |-------------------|
| Brotli    | 92.2733%                  | 0.698939 seconds   | 0.005424 MB          | 4.1315 MB         |
| GZip      | 88.866%                   | 0.016279 seconds   | 0.009512 MB          | 4.1701 MB         |
| LZ4       | 82.307%                   | 0.003267 seconds   | 0.004946 MB          | 4.2319 MB         |
| ZSTD      | 89.3527%                  | 0.003708 seconds   | 0.004996 MB          | 5.069 MB          |
| Snappy    | 79.2932%                  | 0.003156 seconds   | 0.005695 MB          | 4.2631 MB         |


## Performance measuring on a launch js script : Largest one I could find
### Brotli
<img width="998" height="395" alt="image" src="https://github.com/user-attachments/assets/80bd7ba3-d1a2-4fe6-9ca2-571a5a7e68f3" />

### GZip
<img width="994" height="429" alt="image" src="https://github.com/user-attachments/assets/7073e1bf-1fca-40d6-bb00-b909f03911a8" />

### LZ4
<img width="998" height="338" alt="image" src="https://github.com/user-attachments/assets/8304c5ad-7c7e-48cd-bcd0-022a5fd62085" />

### ZSTD
<img width="1003" height="329" alt="image" src="https://github.com/user-attachments/assets/aa93820e-1dd8-4694-8275-205149e076ca" />

### Snappy
<img width="1000" height="392" alt="image" src="https://github.com/user-attachments/assets/96d35099-ad4e-4cf3-aa60-9fb37c24402e" />


### INSIGHTS OF JS
Js Size : 2.57 MB
| Algorithm | Percentage of compression | Time Taken         | Current Memory Usage | Peak Memory Usage |
|-----------|---------------------------|--------------------|--------------------- |-------------------|
| Brotli    | 92.815%                   | 1.746336 seconds   | 0.005794 MB          | 12.899497 MB      |
| GZip      | 89.846%                   | 0.059782 seconds   | 0.009838 MB          | 12.899497 MB      |
| LZ4       | 83.326%                   | 0.007561 seconds   | 0.004590 MB          | 12.899497 MB      |
| ZSTD      | 89.871%                   | 0.008031 seconds   | 0.005216 MB          | 12.899497 MB      |
| Snappy    | 81.125%                   | 0.006897 seconds   | 0.006070 MB          | 12.899497 MB      |

- Brotli: Achieved the **highest compression** at 92.8%, but with the **slowest runtime** at 1.74s — best when you want maximum savings over speed.
- GZip: Delivered **solid compression** at 89.8% with a **very fast** 0.06s runtime — extremely reliable, balanced, and **widely supported**.
- LZ4: **Fastest algorithm** at 0.0075s, but only 83.3% compression — ideal when **speed matters more than size.**
- ZSTD: **Excellent balance** with 89.9% compression at 0.008s — almost as small as Brotli, almost as fast as LZ4. But **not as widely supported**
- Snappy: **Fast** (0.0069s) but **lowest compression** at 81.1% — meant for real-time systems, not maximum reduction.


## Performance measuring on one of our EDS Css
### Brotli
<img width="998" height="385" alt="image" src="https://github.com/user-attachments/assets/86c97a3f-dd9a-4c56-9b53-cc6d59a2a3dd" />

### GZip
<img width="1004" height="424" alt="image" src="https://github.com/user-attachments/assets/9e797702-7b31-459a-bf5c-3304ec563eb9" />

### LZ4
<img width="1004" height="319" alt="image" src="https://github.com/user-attachments/assets/b51a43b2-c7da-401b-a74a-b6c901e12953" />

### ZSTD
<img width="998" height="337" alt="image" src="https://github.com/user-attachments/assets/f31236a3-eb6f-402e-b7ee-4522a5622a00" />

### Snappy
<img width="1005" height="390" alt="image" src="https://github.com/user-attachments/assets/82c3eaba-2ce8-45da-be89-8b5c4cf193cf" />


### INSIGHTS OF CSS
Css Size : 0.172 MB
| Algorithm | Percentage of compression | Time Taken         | Current Memory Usage | Peak Memory Usage |
|-----------|---------------------------|--------------------|--------------------- |-------------------|
| Brotli    | 89.924%                   | 0.133950 seconds   | 0.005526 MB          | 0.366450 MB       |
| GZip      | 87.506%                   | 0.013204 seconds   | 0.009570 MB          | 0.623581 MB       |
| LZ4       | 74.669%                   | 0.000495 seconds   | 0.004836 MB          | 0.392022 MB       |
| ZSTD      | 84.188%                   | 0.000688 seconds   | 0.004945 MB          | 0.521440 MB       |
| Snappy    | 75.654%                   | 0.000523 seconds   | 0.005855 MB          | 0.391345 MB       |

- Brotli: Highest compression at 89.9%, though slower at 0.13s — best when every KB matters for static assets.
- GZip: Strong compression at 87.5% with a quick 0.013s runtime — still a very reliable web default.
- LZ4: Fastest by far at 0.00049s, but weakest compression (74.6%) — ideal for speed-critical scenarios.
- ZSTD: Great balance with 84.2% compression at 0.00068s — nearly LZ4-fast with significantly better reduction.
- Snappy: Similar speed to LZ4 with 75.6% compression — tuned for ultra-low-latency use cases, not size reduction.


## TL;DR Insights
| Algo       | Strength                | Weakness              | Best Use Case              |
| ---------- | ----------------------- | --------------------- | -------------------------- |
| **Brotli** | Best compression        | Relatively much slower when tuned for higher compression                  | Static frontend assets     |
| **GZip**   | Standardized, balanced - Decent compression at decent speeds  | Older algorithm       | APIs, general purpose      |
| **ZSTD**   | Best speed/size balance | Not all browsers support. Can have compatibility issues | Internal systems   |
| **LZ4**    | Fastest algorithm       | Weak compression      | Real-time & streaming data |
| **Snappy** | Very fast & stable      | Lowest compression    | Databases + RPC            |




