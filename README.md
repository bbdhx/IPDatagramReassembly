# Hole-based IP datagram reassembly algorithm
## 1. content
  - Design a reorganization algorithm for IP datagram based on C++ language.
  - Given IP datagram length and MTU,implement sharding of IP datagrams.
  - Send the shards randomly to the destination.
  - The destination reorganization the fragments that arrive in random order based on the "hole" algorithm.

## 1. 内容
  - 基于C++语言设计对IP数据报分片的重组算法。
  - 在给定的IP数据报长度和MTU的情况下，实现对IP数据报的分片。
  - 对分片进行随机顺序发送到目的端。
  - 目的端基于“洞”算法对随机顺序到达的分片进行重组。
 
## 2. illustration
  <img src="/illustration/example01.png" alt="IPDR01" title="IPDR01">  
  <img src="/illustration/example02.png" alt="IPDR02" title="IPDR02">
