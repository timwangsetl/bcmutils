#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Function Name:str_to_arr_i
 *
 * input Parameter: char* str_src -- string source . eg:"1,2,3,4,5,6,"
 * output Parameter: int* arr_des -- int type array destination
 * input Parameter: int num -- The maximum length of the array "arr_des", eg;num = 6
 * 
 *
 * Function description:eg: parse string "1,2,3,4,5,6," to array "arr_des",
 * 									      arr_des[0] = 1, arr_des[1] = 2,
 *  									  arr_des[2] = 3, arr_des[3] = 4,
 *  									  arr_des[4] = 5, arr_des[5] = 6,
 *      
 * Returns: if success return the number of parsing, else return -1;
 * 
 * Author:  shanming.ren
 * Date:    2012-3-29
 *********************Revision History****************
 Date       Version     Modifier       Modifications

 */
int str_to_arr_i(char *str_src, int *arr_des, int num)
{
	int i;
	if(!str_src) {
		return -1;
	}
	if(!arr_des) {
		return -1;
	}
	for (i = 0; i < num; i++) {
		arr_des[i] = 0;
	}
	i = 0;
	while(str_src) {
		if(i < num) {
			arr_des[i++] = atoi(strsep(&str_src, ","));
		} else {
			return i;
		}
		
	}
	return i;
}

/*
 * Function Name:arr_to_str_i
 *
 * input parameter: int[] arr_src -- array source . eg:arr_src[0] = 1, arr_src[1] = 2,
 *												  arr_src[2] = 3, arr_src[3] = 4,
 *												  arr_src[4] = 5, arr_src[5] = 6, 
 * output parameter: char* str_des -- string destination 
 * input parameter: char delimiter -- eg: ','
 * input parameter: int num -- The maximum length of the array "arr_des", eg;num = 6
 * 
 *
 * Function description: parse array "arr_des" (arr_des[0] = 1, arr_des[1] = 2,) to string "1,2,3,4,5,6,"
 * 									           arr_des[2] = 3, arr_des[3] = 4,
 *  									       arr_des[4] = 5, arr_des[5] = 6,
 *  									  
 *      
 * Returns: int
 * 
 * Author:  shanming.ren
 * Date:    2012-3-29
 *********************Revision History****************
 Date       Version     Modifier       Modifications

 */
int arr_to_str_i(int arr_src[], char *str_des, char delimiter, int num)
{
	int i;
	for(i = 0; i < num; i++) {
		sprintf(str_des, "%s%d%c", str_des,arr_src[i], delimiter);
	}
	return 0;
}
/*
 * Function Name:str_to_arr_cfg
 *
 * input parameter: char* str_src -- string source eg:"1,2,3;4,5,6;"
 * output parameter: int* arr_des -- output array
 * input parameter: int inner_layer_num -- ',' numbers in source string
 * input parameter: int outer_layer_num -- ';' numbers in source string
 * 
 *
 * Function description: eg: "1,2,3;4,5,6;" parse to arr_des,
 *							arr_des[0]=1, arr_des[1]=2,
 *							arr_des[2]=3, arr_des[3]=4,
 *							arr_des[4]=5, arr_des[5]=6, 
 *      
 * Returns: int -- return 0
 * 
 * Author:  shanming.ren
 * Date:    2012-3-29
 *********************Revision History****************
 Date       Version     Modifier       Modifications

 */
int str_to_arr_cfg(char *str_src, int *arr_des, int inner_layer_num, int outer_layer_num)
{
	int inner=0, outer=0, ctr = 0;
	char *str_tmp;
	while(str_src) {
		if(outer < outer_layer_num) {
			str_tmp = strsep(&str_src, ";");/* parse inner layer string which  separated by ';'*/
			inner = 0;
			while (str_tmp) {/* parse inner layer string which  separated by ','*/
				if (inner < inner_layer_num) {
					arr_des[ctr++] = atoi(strsep(&str_tmp, ",")); /* The comma between the strings are converted to numerical */
					inner++;
				} else {
					/* end of parsing string between ';' */
					break;
				}
			}
			outer++;
		} else {
			/* end of parsing all string */
			break;
		}
	}
	return 0;
}
