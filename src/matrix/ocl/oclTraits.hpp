# ifndef __OCL_TRAITS_HPP__



  # define __OCL_TRAITS_HPP__
  
  
  
  /**************
   ** includes **
   **************/
  
  // ocl
//  # include "oclDataWrapper.hpp"
  # include "oclGPUDataObject.hpp"
//  # include "oclDataObject.hpp"
    
  
  
  /***********************
   ** struct: oclTraits **
   **   (base struct)   **
   ***********************/
  template <class T>
  struct oclTraits
  {
    /* -- */
  }; // struct oclTraits
  
  
  
  /*******************************
   ** struct: oclTraits <float> **
   **   (single precision)      **
   *******************************/
  template <>
  struct oclTraits <float>
  {
    
    
    /**********************
     ** type definitions **
     **********************/
    typedef float elem_type;
    
    
    /**
     * @brief         Create oclDataObject
     */
    static inline
    oclDataWrapper <elem_type> *
    make_GPU_Obj      (elem_type * const cpu_arg, const size_t & size)
    {
    
      std::cout << "make_GPU_Obj <float>" << std::endl;
    
      return new oclGPUDataObject <elem_type> (cpu_arg, size);
      
    }
    
    
    /**
     * @name          operators
     */
    //@{
    
    /**
     * @brief         elementwise addition
     */
    static inline
    const oclError &
    ocl_operator_add     (oclDataObject * const arg1, oclDataObject * const arg2, oclDataObject * const sum, size_t size)
    {
      
      std::cout << "oclTraits <float> :: ocl_operator_add" << std::endl;
      
      // create array of function arguments
      oclDataObject ** args = (oclDataObject **) malloc (4 * sizeof (oclDataObject *));
      args [0] = arg1;
      args [1] = arg2;
      args [2] = sum;
      args [3] = new oclGPUDataObject <size_t> (& size, 1);
      
      // create function object
      oclFunctionObject * op_obj = oclConnection :: Instance () -> makeFunctionObject ("add", args, 4, oclConnection::KERNEL, oclConnection::SYNC);
      
      // execute function object
      op_obj -> run ();
      
      delete args [3];
      free (args);
      
    }
    
    /**
     * @brief         elementwise subtraction
     */
    static inline
    const oclError &
    ocl_operator_subtract   (oclDataObject * const arg1, oclDataObject * const arg2, oclDataObject * const diff, size_t size)
    {
      
      std::cout << "oclTraits <float> :: ocl_operator_subtract" << std::endl;
      
      // create array of function arguments
      oclDataObject ** args = (oclDataObject **) malloc (4 * sizeof (oclDataObject *));
      args [0] = arg1;
      args [1] = arg2;
      args [2] = diff;
      args [3] = new oclGPUDataObject <size_t> (& size, 1);
      
      // create function object
      oclFunctionObject * op_obj = oclConnection :: Instance () -> makeFunctionObject ("subtract", args, 4, oclConnection::VCL, oclConnection::SYNC);
      
      // execute function object
      op_obj -> run ();
      
      delete args [3];
      free (args);
      
    }
    
    //@}
    
    
  };
  
  
  
  /********************************
   ** struct: oclTraits <size_t> **
   **   (size_t)                 **
   ********************************/
  template <>
  struct oclTraits <size_t>
  {
    
    
    /**********************
     ** type definitions **
     **********************/
    typedef size_t elem_type;
    
    
    /**
     * @brief         Create oclDataObject
     */
    static inline
    oclDataWrapper <elem_type> *
    make_GPU_Obj      (elem_type * const cpu_arg, const size_t & size)
    {
    
      std::cout << "make_GPU_Obj <size_t>" << std::endl;
    
      return new oclGPUDataObject <elem_type> (cpu_arg, size);
      
    }
    
    
  };
  
  
  
# endif // __OCL_TRAITS_HPP__
