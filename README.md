# Lossless Compression Algorithms Testing : 
Playing around with the most common lossless compression algorithms. Was interested in how they perform for our use cases, so tried profiling the performance of the algorithms to get real-world insights.

Rough test results and insights mentioned below. Structured results present in our blog.


## Brotli Grid search on tunable parameters : 

| Quality | Mode | Lgwin | Compression Percentage | Time Taken | Peak Memory Usage | Compression Ratio |
|---------|------|-------|------------------------|------------|-------------------|-------------------|
| 0 | 1 | 23 |   91.10% |   0.001210s |     0.002618 MB |  91.0992 |
| 1 | 0 | 22 |   92.16% |   0.001427s |     0.002170 MB |  92.1617 |
| 11 | 2 | 19 |   95.60% |   0.335548s |     0.002226 MB |  95.6035 |

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


## Conclusions/ Learnings
| Algo       | Strength                | Weakness              | Best Use Case              |
| ---------- | ----------------------- | --------------------- | -------------------------- |
| **Brotli** | Best compression        | Slow                  | Static frontend assets     |
| **GZip**   | Standardized, balanced  | Older algorithm       | APIs, general purpose      |
| **ZSTD**   | Best speed/size balance | Not browser-supported | Modern backend workloads   |
| **LZ4**    | Fastest algorithm       | Weak compression      | Real-time & streaming data |
| **Snappy** | Very fast & stable      | Lowest compression    | Databases + RPC            |


### Insights 




