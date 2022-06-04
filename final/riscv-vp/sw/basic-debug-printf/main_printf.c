#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "unistd.h"
#include "math.h"
int x = 5;
int y;

int f(int a, int b) {
	if (a > 0) {
		int ans = a + b;
		return ans;
	}
	a = -a;
	return b + a;
}
//Convert a float into a string
//Only 4 digits of fractional parts are converted
//If more digits are needed, please modify the tmpInt2 scaling
void sprintfloat(char* buf, float num) {
	char* tmpSign = (num < 0) ? "-" : "";
	float tmpVal = (num < 0) ? -num : num;

	int tmpInt1 = (int) tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
	int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer (123).

	// Print as parts, note that you need 0-padding for fractional bit.
	// sprintf(buf, "%s%d.%04d", tmpSign, tmpInt1, tmpInt2);
	sprintf(buf, "%s%d.%04d\n", tmpSign, tmpInt1, tmpInt2);
}

int main(int argc, char **argv) {
	char buf[256];
	int a = 5;
	int b = 3;
	b--;
	a = a / b;
	a = a * 2;
	a = f(x, y);
	printf("1 a=%d\n", a);	
	//Demo float print
	float c = 2.0;
	float d = 5.0;
	float e = d/c;
	sprintfloat(buf, e);
    printf("2 e=%s", buf);
	return 0;
}
