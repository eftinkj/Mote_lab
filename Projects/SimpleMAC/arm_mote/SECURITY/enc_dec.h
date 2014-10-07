void encrypt(){
  int status;
  char i;

  /************** 2. creation of the encryption key **************/
  keyInst.blockLen = BLOCK_LENGTH;
  status = makeKey(DIR_ENCRYPT);

  /************** 3. initialisation of the cipher *********/
  cipherInst.blockLen = BLOCK_LENGTH;
  status = cipherInit (MODE_ECB, NULL);

  /************** 4. encryption of the block ***********************/
  memset(buff1,0,BLOCK_SIZE);
  for (i=0;i<BLOCK_SIZE;i++) blockE[i]=blockToEncrypt[i];

  status = blockEncrypt();

  for (i=0;i<BLOCK_SIZE;i++) {buff1[i]=blockE[i]; printf("%c",buff1[i]);}
  
  printf("\n");
  return;}

 void decrypt(){
  int status;
  char i;

  /************** 5. creation of the decryption key ****************/
  status = makeKey(DIR_DECRYPT);

  /************** 6. decryption of the block ***********************/
  memset(buff2,0,BLOCK_SIZE);
//  for (i=0;i<BLOCK_SIZE;i++) blockD[i]=blockE[i];

  status = blockDecrypt();

  for (i=0;i<BLOCK_SIZE;i++) {buff2[i]=blockD[i];	printf("%c",buff2[i]);}
  
  return;
}