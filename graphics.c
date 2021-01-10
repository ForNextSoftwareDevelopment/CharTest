#include "def.h"
#include "graphics.h"

/* error-code   BASIC error
** ----------   -----------
**       1  =   too many files
**       2  =   file open
**       3  =   file not open
**       4  =   file not found
**       5  =   device not present
**       6  =   not input-file
**       7  =   not output-file
**       8  =   missing file-name
**       9  =   illegal device-number
**
**      10  =   STOP-key pushed
**      11  =   general I/O-error
*/

/* Set default characters and disable sprites */
void SetDefaultChars()
{
    /* Set addresses of control registers */
    char* ptr6510BankSelect               = (char*) 0x0001;
    char* ptr6510BankSelectScreen         = (char*) 0x0288;
    char* ptrVicIIBankSelect              = (char*) 0xDD00;
    char* ptrVicIIMemoryControlRegister   = (char*) 0xD018; 
    char* ptrDataDirectionRegisterPortA   = (char*) 0xDD02;

    /* Set data direction port A to output */
    *ptrDataDirectionRegisterPortA = *ptrDataDirectionRegisterPortA | 0b00000011;

    /* Set VicII bank */
    *ptrVicIIBankSelect = *ptrVicIIBankSelect & 0b11111100 | 0b00000011;

    /* Set use of original character set (set address of character set to use by the system) */
    *ptrVicIIMemoryControlRegister = *ptrVicIIMemoryControlRegister & 0b11110000 | 0b00000100;

    /* Set screen (set address of screen to use by the vicII) */
    *ptrVicIIMemoryControlRegister = *ptrVicIIMemoryControlRegister & 0b00001111 | 0b00010000;

    /* Set screen (set address of screen to use by the 6510) */
    *ptr6510BankSelectScreen = 0b00000100;
}

/* Set custom characters (at 0xC800)*/
void SetCustomChars()
{
    /* Set addresses of control registers */
    char* ptr6510BankSelect               = (char*) 0x0001;
    char* ptr6510BankSelectScreen         = (char*) 0x0288;
    char* ptrVicIIBankSelect              = (char*) 0xDD00;
    char* ptrVicIIMemoryControlRegister   = (char*) 0xD018; 
    char* ptrDataDirectionRegisterPortA   = (char*) 0xDD02;

    /* Set data direction port A to output */
    *ptrDataDirectionRegisterPortA = *ptrDataDirectionRegisterPortA | 0b00000011;

    /* Set VicII bank */
    *ptrVicIIBankSelect = *ptrVicIIBankSelect & 0b11111100 | 0b00000000;

    /* Set use of custom character set (set address of character set to use by the vicII) */
    *ptrVicIIMemoryControlRegister = *ptrVicIIMemoryControlRegister & 0b11110000 | 0b00000010;

    /* Set screen (set address of screen to use by the vicII) */
    *ptrVicIIMemoryControlRegister = *ptrVicIIMemoryControlRegister & 0b00001111 | 0b00000000;

    /* Set screen (set address of screen to use by the 6510) */
    *ptr6510BankSelectScreen = 0b11000000;
}

/* Load custom chars from file */
bool LoadCustomChars()
{
    const char lfn_command = 15;
    const char device_command = 8;
    const char sa_command = 15;
    const char* command = "r:daredevil.64c=daredevil.64c";
    const char command_size = 29;

    char message[41];

    const char lfn = 2;
    const char device = 8;
    const char sa = 2;
    const char* filename = "daredevil.64c";

    unsigned char *data = (unsigned char *)custom_charset_mem_location;
    
    char *errmsg;
    unsigned char retval;
    unsigned int i;

    printf("\r\nReading data file '%s'\r\n", filename);

    /* Clear character memory (debug purpose) */
    for(i=0; i < 2048; i++)
    {
        data[i] = 0x00;
    }

    /* Set up a logical file (command channel) */
    cbm_k_setlfs(lfn_command, device_command, sa_command);       

    /* Set filename parameters (dummy) */     
    cbm_k_setnam("");

    /* Check if the file is present on disk (try to rename with the same name and look for return code */
    retval = cbm_k_open();
    if(retval != 0) 
    {
        /* Close logical file */
        cbm_k_close(lfn_command);

        /* Show error */    
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError while opening a logical file\r\nfor the command output channel\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);

        return false;
    }

    retval = cbm_k_ckout(lfn_command);
    if(retval != 0) 
    {
        /* Close logical file */
        cbm_k_close(lfn_command);

        /* Show error */    
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError while designating a logical file\r\nas the current command output channel\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);

        return false;
    }

    /* Output command to drive */
    for (i=0; (i<command_size) && (retval == 0); i++)
    {
        cbm_k_bsout(command[i]);
        retval = cbm_k_readst();
    }

    if (retval != 0)
    {
        /* Close logical file */
        cbm_k_close(lfn_command);

        /* Show error */    
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError output command to drive:\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);
        
        return false;
    } 

    /* Send untalk */
    cbm_k_clrch();

    retval = cbm_k_chkin(lfn_command);
    if(retval != 0) 
    {
        /* Close logical file */
        cbm_k_close(lfn_command);

        /* Show error */    
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError while designating a logical file\r\nas the current command input channel\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);

        return false;
    }

    /* Read response until end of file or error (or buffer is full) */
    i=0;
    do
    {
        message[i++] = cbm_k_basin();
        retval = cbm_k_readst();
    } while (i<sizeof(message) && (retval == 0));

    /* Close string */
    i > 0 ? message[i-1] = 0x00: message[i] = 0x00;

    /* Send unlisten */
    cbm_k_clrch();

    /* Close logical file */
    cbm_k_close(lfn_command);

    /* Give error if status is not EOF */
    if ((retval & 0xBF) != 0)
    {
        /* Show error */    
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError reading response from drive:\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);
        
        return false;
    } 

    /* Check response to see if the file exists */
    if ((message[0] != '0') || (message[1] != '0')) 
    {
        /* Show error */    
        printf("\r\nError:\r\n%s\r\n", message);
        
        return false;
    }

    /* Set up a logical file */
    cbm_k_setlfs(lfn, device, sa);       

    /* Set filename parameters */     
    cbm_k_setnam(filename);

    /* Open a logical I/O file (returns error code) */
    retval = cbm_k_open();
    if (retval != 0)
    {
        /* Close logical file */
        cbm_k_close(lfn);

        /* Show error */    
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError opening data file:\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);
        
        return false;
    }   

    /* Read status for error */    
    retval = cbm_k_readst();
    if (retval != 0)
    {
        /* Close logical file */
        cbm_k_close(lfn);

        /* Show error */    
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError opening data file:\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);
        
        return false;
    }   

    /* Designate a logical file as the current input channel */
    retval = cbm_k_chkin(lfn);
    if(retval != 0) 
    {
        /* Close logical file */
        cbm_k_close(lfn);

        /* Show error */    
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError while designating a logical file\r\nas the current input channel\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);

        return false;
    }

    /* Read file */
    for(i=0; (i < 2048) && (retval == 0); i++)
    {
        /* Get one byte from the input device */
        data[i] = cbm_k_basin();

        retval = cbm_k_readst();
        if ((retval  & 0xBF) != 0) break;
    }
    
    /* Close logical file */
    cbm_k_close(lfn);

    /* Show error */    
    if ((retval & 0xBF) != 0) 
    {
        errmsg = (char *) _stroserror(retval);
        printf("\r\nError while reading a logical file\r\nCode = %d\r\nError = '%s'\r\n", retval, errmsg);
        return false;
    }

    /* Show warning */    
    if (((retval & 0x40) != 0) && (i != 2048))
    {
        printf("\r\nFile too short for full set:\r\nread %d bytes\r\n", i);
        return true;
    }

    printf("\r\nRead %d bytes\r\n", i);   

    return true;
}

/* Print all character codes (debug) */
void PrintCustomCharsCode()
{ 
    unsigned int i;
    unsigned int j;
    unsigned char *data = (unsigned char *)custom_charset_mem_location;

    for(i=0; i < 256; i++)
    {
        printf("\r\n");
        printf("%04d - ", i * 8);
        
        for(j=0; j < 8; j++)
        {
            printf("%02x ", data[(i * 8) + j]);
        }
    }
}
