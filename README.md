# Hole-based IP dategram reassembly algorithm

## 1. 内容
  - 基于C++语言设计对IP数据报分片的重组算法。
  - 在给定的IP数据报长度和MTU的情况下，实现对IP数据报的分片。
  - 对分片进行随机顺序发送到目的端。
  - 目的端基于“洞”算法对随机顺序到达的分片进行重组。
 
## 2. illustration
  <img src="/IPDatagramReassembly/illustration/eample01.png" alt="IPDR01" title="IPDR01">
