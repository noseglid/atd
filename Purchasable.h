#ifndef __PURCHASABLE_H__
#define __PURCHASABLE_H__

class Purchasable
{

protected:
	Purchasable(int price) : price(price)
	{
	}

public:
	int price;
};

#endif
