/********以下是wave格式文件的文件头格式说明******/  
/*------------------------------------------------ 
 * |             RIFF WAVE Chunk                  | 
 * |             ID = 'RIFF'                     | 
 * |             RiffType = 'WAVE'                | 
 * ------------------------------------------------ 
 *  |             Format Chunk                     | 
 *  |             ID = 'fmt '                      | 
 *  ------------------------------------------------ 
 *  |             Fact Chunk(optional)             | 
 *  |             ID = 'fact'                      | 
 *  ------------------------------------------------ 
 *  |             Data Chunk                       | 
 *  |             ID = 'data'                      | 
 *  ------------------------------------------------*/  
/**********以上是wave文件格式头格式说明***********/  
/*wave 文件一共有四个Chunk组成，其中第三个Chunk可以省略，每个Chunk有标示（ID）, 
 * 大小（size,就是本Chunk的内容部分长度）,内容三部分组成*/  
struct WaveHeader {
  // RIFF WAVE CHUNK
  unsigned char a[4];//四个字节存放'R','I','F','F'  
  long int b;        //整个文件的长度-8;每个Chunk的size字段，都是表示除了本Chunk的ID和SIZE字段外的长度;  
  unsigned char c[4];//四个字节存放'W','A','V','E'  
  
  // Format CHUNK
  unsigned char d[4];//四个字节存放'f','m','t',''  
  long int e;       //16后没有附加消息，18后有附加消息；一般为16，其他格式转来的话为18  
  short int f;       //编码方式，一般为0x0001;  
  short int g;       //声道数目，1单声道，2双声道;  
  long int h;        //采样频率;  
  long int i;        //每秒所需字节数;  
  short int j;       //每个采样需要多少字节，若声道是双，则两个一起考虑;  
  short int k;       //即量化位数  

  // Data Chunk
  unsigned char p[4];//四个字节存放'd','a','t','a'  
  long int q;        //语音数据部分长度，不包括文件头的任何部分  
};
