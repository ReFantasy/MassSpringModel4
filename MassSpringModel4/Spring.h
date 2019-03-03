#ifndef __SPRING_H__
#define __SPRING_H__
class Spring
{
public:
	// 弹簧两端连接的质点
	int imass1;
	int imass2;

	// 弹簧张量
	float tension; // 单位：牛每米

	float springConstant;  // 胡克系数
	float naturalLength;   // 弹簧的自然长度

};

#endif //__SPRING_H__
