#ifndef __IO_TYPES_H__
#define __IO_TYPES_H__

/**
 * @file     io_types.h
 * @brief    IO Type Definitions for embedded development
 * @version  V1.0.0
 * @date     2024-06-11
 */

/*----------------------------------------------------------------------------
  IO Type Definitions
 *----------------------------------------------------------------------------*/
#ifndef __IO
#define __IO volatile             /*!< Defines 'read / write' permissions */
#endif

#ifndef __O
#define __O volatile              /*!< Defines 'write only' permissions */
#endif

#ifndef __I
#define __I volatile const        /*!< Defines 'read only' permissions */
#endif

#endif /* __IO_TYPES_H__ */ 