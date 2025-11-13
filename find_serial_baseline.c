// program tries all passwords of certain length until it finds one that
// produces the searched for hash value
//
// compile with
// gcc -O2 find.c -lcrypt -o find.x

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

int main(void)
{
  /* password hash (with salt), for hashes below password has 3 characters  */
  const char *const hash = "$1$WicCakKA$vKI5prjBi7KzmR3maGrza1";
//  const char *const hash = "$1$zkcCa.NA$s7K1PdYGSTF5xUngmCAHM.";
//  const char *const hash = "$1$FlcCaENA$CQ2dmLurILPF3AqEMG2rt/";
//  const char *const hash = "$1$SlcCaRNA$zZbCNhZytQGwArj1dhL7U0";

  /* password hash (with salt), for hashes below password has 4 characters  */
// const char *const hash = "$1$JO/EajqE$TZIldQ3Bc/E83rDfV0lCs1";

  int num_pass_chars=64; //number of characters allowed in password
  int pass_len=3; // length of password
  char passwd[pass_len+1];
  const char *const passchars =  //characters allowed in password
    "./0123456789ABCDEFGHIJKLMNOPQRST"
    "UVWXYZabcdefghijklmnopqrstuvwxyz";

  long long int number_of_possible_passwords;
  long long int ilong,itest;
  int j;

  char *result;
  int ok;

// compute total number of possible passwords
  number_of_possible_passwords=1;
  for(j=0;j<pass_len;j++) number_of_possible_passwords*=num_pass_chars;


// loop through all possible passords
  for(ilong=0;ilong<number_of_possible_passwords;ilong++){

    itest=ilong;
    for(j=0;j<pass_len;j++){
      passwd[j]=passchars[itest%num_pass_chars];
      itest=itest/num_pass_chars;
    }
    passwd[j]='\0'; //add null character to terminate the string

    result = crypt(passwd, hash);

    /* Test the result. */
    ok = strcmp (result, hash) == 0;

    if(ok) {
    printf(passwd);
    printf("\n is the password \n");
    return 0;
    }


  }

}

