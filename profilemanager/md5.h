#ifndef MD5_H
#define MD5_H

#include <ostream>

class MD5Digest
{
	public:
	MD5Digest();
	MD5Digest(const char *message,long length);
	MD5Digest(const MD5Digest &other);
	void Update(const char *message,long length);
	MD5Digest &operator=(const MD5Digest &other);
	bool operator==(const MD5Digest &other);
	bool operator!=(const MD5Digest &other);
	private:
	unsigned char digest[16];
	friend std::ostream& operator<<(std::ostream &s,const MD5Digest &c);
};

#endif

