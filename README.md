ip2loc是一个根据ip查询归属地的php扩展，包含两个部分：loader工具以及用于php的查询扩展。loader工具将ip归属地数据加载入共享内存；php扩展提供两组函数从共享内存中查询ip：

<pre>
<code>
&lt;?php
/**
 * array ip2loc(string $ip);
 * 一次ip2loc会有一次完整的共享内存的attach和detach
 */
$ip = '202.108.37.12';
var_dump(ip2loc($ip));

/**
 * attach共享内存
 * shm resource ip2loc_connect();
 * 查询ip
 * array ip2loc_get(string $ip, shm resource $shm);
 * detach共享内存
 * boolean ip2loc_close(shm resource $shm);
 * 适用于脚本中查询多个ip的情况
 */
$shm = ip2loc_connect();
ip2loc_get($ip, $shm);
ip2loc_close($shm);
?&gt;
</code>
</pre> 


---

## 安装 ##

<pre>
<code>
#loader部分
touch /dev/shm/ip2loc
cd loader
make
#测试
./ip2loc_loader region_only_human.txt
./ip2loc 202.108.37.12

#php扩展部分
phpize
./configure
make
</code>
</pre>

---

## 性能 ##

在一台双核1.86GHz的测试机上跑150w左右数据，耗时30s，即每秒5w左右处理能力，我未进行详尽测试，处理能力应该与机器性能有直接关系。该扩展工作原理为在共享内存中使用二分查找定位记录，未使用缓存，所以可以使用重复的数据进行测试。

---

数据源为一份网上公开的国内ip归属地文本文件，ip2loc依赖于该文件的格式工作。

这份代码无任何copyright要求，如果有需要，任何人可以以任何形式修改、分发，无需任何声明。希望能对大家有用。