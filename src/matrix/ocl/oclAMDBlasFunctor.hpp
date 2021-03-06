# ifndef __OCL_AMD_BLAS_FUNCTOR_HPP__




  /************
   ** makros **
   ************/
  # define __OCL_AMD_BLAS_FUNCTOR_HPP__



  /**************
   ** includes **
   **************/
   
  // ocl
  # include "oclSettings.hpp"
  # include "oclAMDBlasTraits.hpp"
  
  // AMD BLAS
  # include <clAmdBlas.h>




  /**************************
   ** forward declarations **
   **************************/
  template <class T, class S>
  class oclAMDBlasObject;




  /******************************
   ** class: oclAMDBlasFunctor **
   **   (interface)            **
   **   (still containing      **
   **    dummy functions)      **
   ******************************/
  template <class T, class S>
  class oclAMDBlasFunctor
  {
  
  

    public:


      /**
       * @brief               overloaded operator (functor)
       */
      inline
      virtual
      void
      operator()              ()
      const
      {
      
        std::cout << "not implemented" << std::endl;
        
      }
    
    

    protected:
    
    
      /**
       * @name                constructors and destructors
       */
      //@{
      
      
      /**
       * @brief               constructor
       */
      oclAMDBlasFunctor          ( const oclAMDBlasObject <T, S> * const p_amdObj )
                                 : mp_amdObj (p_amdObj)
      {
      
        print_optional ("Ctor: \"oclAMDBlasFunctor\"", VERB_HIGH);
        
      }
      
      
      //@}
      
      
      /**********************
       ** member variables **
       **********************/
      
      const oclAMDBlasObject <T, S> * const mp_amdObj;


      /**********************
       ** static variables **
       **********************/
      static
      const VerbosityLevel op_v_level = VERB_MIDDLE;



  }; // class oclAMDBlasFunctor
  


  
  /****************************
   ** class: amdBlasGEMM     **
   **    (derived)           **
   ****************************/
  template <class T, class S>
  class amdBlasGEMM : public oclAMDBlasFunctor <T, S>
  {

  
  
    public:

    
      /**
       * @name              constructors and destructors
       */
      //@{
      
      
      /**
       * @brief             constructor
       */
      amdBlasGEMM         ( const oclAMDBlasObject <T, S> * const p_amdObj )
                           : oclAMDBlasFunctor <T, S> (p_amdObj)
      {

        print_optional ("Ctor: \"amdBlasGEMM\"", VERB_HIGH);

      }
      
      ~amdBlasGEMM ()
      { /* -- */ }
      
      
      //@}
      
      
      /**
       * @brief             refer to base class
       */
      inline
      virtual
      void
      operator()            ()
      const
      {

        print_optional ("amdBlasGEMM <T, S> :: operator()", oclAMDBlasFunctor <T, S> :: op_v_level);

        /**
         * get memory objects
         */
        cl_mem mat1 = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getMemObject (0);
        cl_mem mat2 = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getMemObject (1);
        cl_mem result = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getMemObject (2);
        
        /**
         * get scalar arguments
         */
        int m = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getScalarArg (0);
        int n = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getScalarArg (1);
        int k = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getScalarArg (2);
        int transA = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getScalarArg (3);
        int transB = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getScalarArg (4);
        
        // transposition
        clAmdBlasTranspose tr_A, tr_B;
        switch (transA)
        {
          case 0: tr_A = clAmdBlasNoTrans;   break;
          case 1: tr_A = clAmdBlasTrans;     break;
          case 2: tr_A = clAmdBlasConjTrans; break;
        }
        switch (transB)
        {
          case 0: tr_B = clAmdBlasNoTrans;   break;
          case 1: tr_B = clAmdBlasTrans;     break;
          case 2: tr_B = clAmdBlasConjTrans; break;
        }
        
        // factors
        typename amdBlasTraits <T> :: cl_type one  = amdBlasTraits <T> :: One ();
        typename amdBlasTraits <T> :: cl_type zero = amdBlasTraits <T> :: Zero ();
        
        // get command queue
        cl_command_queue comq = oclConnection :: Instance () -> getCommandQueue ();

        // create ocl event object
        cl_event event = NULL;

        // prepare AMD Blas
        clAmdBlasSetup();
        
        cl_int code =
        amdBlasTraits <T> :: GEMM (clAmdBlasColumnMajor,
                                   tr_A,
                                   tr_B,
                                   m, n, k,
                                   one,  mat1, m,
                                         mat2, k,
                                   zero, result, m,
                                   1 , & comq,
                                   0 , NULL, &event);
                       
        // Wait for calculations to be finished.
        clWaitForEvents(1, &event);
        
        // finalize AMD Blas
        clAmdBlasTeardown();
                        
      }
  
  
  
  }; // class amdBlasGEMM




  /****************************
   ** class: amdBlasGEMV     **
   **    (derived)           **
   ****************************/
  template <class T, class S>
  class amdBlasGEMV : public oclAMDBlasFunctor <T, S>
  {

  
  
    public:

    
      /**
       * @name              constructors and destructors
       */
      //@{
      
      
      /**
       * @brief             constructor
       */
      amdBlasGEMV         ( const oclAMDBlasObject <T, S> * const p_amdObj )
                           : oclAMDBlasFunctor <T, S> (p_amdObj)
      {

        print_optional ("Ctor: \"amdBlasGEMV\"", VERB_HIGH);

      }
      
      ~amdBlasGEMV ()
      { /* -- */ }
      
      
      //@}
      
      
      /**
       * @brief             refer to base class
       */
      inline
      virtual
      void
      operator()            ()
      const
      {

        print_optional ("amdBlasGEMV <T, S> :: operator()", oclAMDBlasFunctor <T, S> :: op_v_level);

        /**
         * get memory objects
         */
        cl_mem mat1 = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getMemObject (0);
        cl_mem mat2 = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getMemObject (1);
        cl_mem result = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getMemObject (2);
        
        /**
         * get scalar arguments
         */
        int m = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getScalarArg (0);
        int n = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getScalarArg (1);
        int transA = oclAMDBlasFunctor <T, S> :: mp_amdObj -> getScalarArg (2);
        
        // transposition
        clAmdBlasTranspose tr_A;
        switch (transA)
        {
          case 0: tr_A = clAmdBlasNoTrans;   break;
          case 1: tr_A = clAmdBlasTrans;     break;
          case 2: tr_A = clAmdBlasConjTrans; break;
        }
        
        /**
         * offset, increment
         */
        int offset = 0;
        int inc = 1;
        
        // factors
        typename amdBlasTraits <T> :: cl_type one  = amdBlasTraits <T> :: One ();
        typename amdBlasTraits <T> :: cl_type zero = amdBlasTraits <T> :: Zero ();
        
        // get command queue
        cl_command_queue comq = oclConnection :: Instance () -> getCommandQueue ();

        // create ocl event object
        cl_event event = NULL;

        // prepare AMD Blas
        clAmdBlasSetup();
        
        cl_int code =
        amdBlasTraits <T> :: GEMV (clAmdBlasColumnMajor,
                                   tr_A,
                                   m, n,
                                   one,  mat1, m,
                                         mat2,
                                   offset, inc,
                                   zero, result,
                                   offset, inc,
                                   1 , & comq,
                                   0 , NULL, &event);
                       
        // Wait for calculations to be finished.
        clWaitForEvents(1, &event);
        
        // finalize AMD Blas
        clAmdBlasTeardown();
                        
      }
  
  
  
  }; // class amdBlasGEMV




  /**********************
   ** global functions **
   **********************/
  template <class T, class S>
  static
  const oclAMDBlasFunctor <T, S> * const
  get_amdBlas_functor     (const   oclAMDBlasType        &           algo,
                           const oclAMDBlasObject <T, S> * const p_amdObj)
  {
        
    print_optional (" :: get_amdBlas_functor", VERB_HIGH);

    /* choose requested algorithm */
    switch (algo)
    {
    
      case amdblasGEMM:
        return new amdBlasGEMM <T, S> (p_amdObj);

      case amdblasGEMV:
        return new amdBlasGEMV <T, S> (p_amdObj);

      default:
        throw "*!* Requested AMD Blas algorithm is not available! *!*";
    
    }

  }
  
  
  
  
# endif // __OCL_AMD_BLAS_FUNCTOR_HPP__
