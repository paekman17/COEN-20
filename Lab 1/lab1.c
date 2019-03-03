/*
Ethan Paek
COEN 20 Lab #1
This is a program that will utilize 2's Complement and be able to interchange between binary and decimal numbers
*/
void TwosComplement(const int input[8], int output[8]){
	int firstOne = 0; //keep track of when we hit the first one
	for(int i = 0; i < 8; i++){
		if(firstOne == 0){ //copy everything up until the first one
			if(input[i] == 0){
				output[i] = 0;
			}
			else{
				output[i] = 1;
				firstOne = 1;
			}
		}
		else{ //now copy everything else opposite of the input
			if(input[i] == 0){
				output[i] = 1;
			}
			else{
				output[i] = 0;
			}
		}
	}
	return;
}

float Bin2Dec(const int bin[8]){
	float polynomial = 0;
	for(int i = 7; i >= 0; i--){ //follow the pseudocode as mentioned in lab 1 manual
		polynomial = ((2*polynomial) + bin[i]);
	}
	polynomial /= 128.0; //since we are expecting 7 decimal points, we must divide by 2^7
	if(bin[7] == 1){
		polynomial -= 2.0;
	}
	return polynomial;
}

void Dec2Bin(const float x, int bin[8]){
    int i;
    int temp[8];
    float fNum = (x > 0) ? x : -x; //get absolute value of float
    fNum = 128.0 * fNum; //convert number to integer as we did earlier
    if((int) fNum < 127) { //must round up to nearest integer as Dr. Lewis mentioned in class
        fNum = fNum + 0.5;
    }
    int num = (int) fNum; //binary representation of this number is the result if the original number is positive
    if(num == 128){ //take care of overflow case: if the number is 128, repeated division does not work
        bin[7] = 1; //this is essentially for the case of -1.0
        for(i = 6; i >= 0; i--) {
            bin[i] = 0;
        }
    }
    else { //Repeated division method to get bits
        i = 0;
        while (i < 7) {
            temp[i] = num % 2;
            num = num / 2;
            i++;
        }
        temp [7] = 0;
        if(x < 0) { //If original number was negative, take 2's complement, else copy the bits
            TwosComplement(temp,bin);
        }
	else {
	    i = 0;
            while (i < 8) {
                bin[i] = temp[i];
                i++;
            }
        }
    }
    return;
}
