/* ActiveConstReference.h -- Const reference to an active element of an array

    Copyright (C) 2015-2017 European Centre for Medium-Range Weather Forecasts

    Author: Robin Hogan <r.j.hogan@ecmwf.int>

    This file is part of the Adept library.


   Provide an active scalar type where the data is actually a
   reference to an element of array. This enables an active array to
   be indexed such that the returned value can be used as an r-value
   and participate in expressions to be differentiated.

*/

#ifndef AdeptActiveConstReference_H
#define AdeptActiveConstReference_H

#include <iostream>
#include <vector>

#include <adept/Active.h>

namespace adept {

  // ---------------------------------------------------------------------
  // Definition of ActiveReference class
  // ---------------------------------------------------------------------
  template <typename Type>
  class ActiveConstReference : public Expression<Type, ActiveConstReference<Type> > {
    // CONTENTS
    // 1. Preamble
    // 2. Constructors
    // 3. Operators
    // 4. Public member functions that don't modify the object
    // 5. Public member functions that modify the object
    // 6. Protected member functions
    // 7. Data

  public:
    // -------------------------------------------------------------------
    // 1. Preamble
    // -------------------------------------------------------------------

    // Static definitions to enable the properties of this type of
    // expression to be discerned at compile time
    static const bool is_active = true;
    static const int  rank      = 0;
    static const int  n_active  = 1 + internal::is_complex<Type>::value;
    static const int  n_arrays  = 0;
    static const int  n_scratch = 0;

    // -------------------------------------------------------------------
    // 2. Constructors
    // -------------------------------------------------------------------

  private:
    // There is only one way to construct an ActiveConstReference, so all
    // others that would otherwise be generated by the compiler are
    // made inaccessible
    ActiveConstReference() { }

  public:

    ActiveConstReference(const ActiveConstReference& rhs)
      : val_(rhs.value()), gradient_index_(rhs.gradient_index()) { }
    
    // In order to initialize this object, we pass in the gradient
    // index from the location in the array as the first argument.
    ActiveConstReference(const Type& val, Index gradient_index)
      : val_(val), gradient_index_(gradient_index) { 
    }

    /*
    ActiveConstReference(const ActiveConstReference& rhs)
      : val_(const_cast<ActiveConstReference<Type>&>(rhs).lvalue()),
	gradient_index_(rhs.gradient_index()) { }
    */

    // Destructor does not unregister the object from the stack since
    // it is not the only reference to it.
    ~ActiveConstReference() { }


    // -------------------------------------------------------------------
    // 3. Operators
    // -------------------------------------------------------------------
	   
    // Assignment operator with an active variable on the rhs: first a
    // non-template version because otherwise compiler will generate
    // its own; must be inaccessible
  private:
    ActiveConstReference& operator=(const ActiveConstReference& rhs) { }

  public:
    // -------------------------------------------------------------------
    // 4. Public member functions that don't modify the object
    // -------------------------------------------------------------------

    // Get the underlying passive value of this object
    const Type& value() const {
      return val_; 
    }

    // Get the index of the gradient information for this object
    const Index& gradient_index() const { return gradient_index_; }

    // If an expression leads to calc_gradient being called on an
    // active object, we push the multiplier and the gradient index on
    // to the operation stack (or 1.0 if no multiplier is specified)
    template <int Rank>
    void calc_gradient(Stack& stack, const ExpressionSize<Rank>&) const {
      stack.push_rhs(1.0, gradient_index_);
    }

    template <int Rank, typename MyType>
    void calc_gradient(Stack& stack, const MyType& multiplier, 
		       const ExpressionSize<Rank>&) const {
      stack.push_rhs(multiplier, gradient_index_);
    }

    // Set the value of the gradient, for initializing an adjoint;
    // note that the value of the gradient is not held in the active
    // object but rather held by the stack
    template <typename MyType>
    void set_gradient(const MyType& gradient) const {
      return ADEPT_ACTIVE_STACK->set_gradients(gradient_index_,
					       gradient_index_+1, 
					       &gradient);
    }

    // Get the value of the gradient, for extracting the adjoint after
    // calling reverse() on the stack
    template <typename MyType>
    void get_gradient(MyType& gradient) const {
      return ADEPT_ACTIVE_STACK->get_gradients(gradient_index_,
					       gradient_index_+1, &gradient);
    }
    Type get_gradient() const {
      Type gradient = 0;
      ADEPT_ACTIVE_STACK->get_gradients(gradient_index_,
					gradient_index_+1, &gradient);
      return gradient;
    }
 

    // For modular codes, some modules may have an existing
    // Jacobian code and possibly be unsuitable for automatic
    // differentiation using Adept (e.g. because they are written in
    // Fortran).  In this case, we can use the following two functions
    // to "wrap" the non-Adept code.

    // Suppose the non-adept code uses the double values from n aReal
    // objects pointed to by "x" to produce a single double value
    // "y_val" (to be assigned to an aReal object "y"), plus a pointer
    // to an array of forward derivatives "dy_dx".  Firstly you should
    // assign the value using simply "y = y_val;", then call
    // "y.add_derivative_dependence(x, dy_dx, n);" to specify how y
    // depends on x. A fourth argument "multiplier_stride" may be used
    // to stride the indexing to the derivatives, in case they are
    // part of a matrix that is oriented in a different sense.
    void add_derivative_dependence(const Active<Type>* rhs,
				   const Real* multiplier,
				   int n, 
				   int multiplier_stride = 1) const {
#ifdef ADEPT_RECORDING_PAUSABLE
      if (ADEPT_ACTIVE_STACK->is_recording()) {
#endif
#ifndef ADEPT_MANUAL_MEMORY_ALLOCATION
	// Check there is space in the operation stack for n entries
	ADEPT_ACTIVE_STACK->check_space(n);
#endif
	for (int i = 0; i < n; i++) {
	  Real mult = multiplier[i*multiplier_stride];
	  if (mult != 0.0) {
	    // For each non-zero multiplier, add a pseudo-operation to
	    // the operation stack
	    ADEPT_ACTIVE_STACK->push_rhs(mult,
					 rhs[i].gradient_index());
	  }
	}
	ADEPT_ACTIVE_STACK->push_lhs(gradient_index_);
#ifdef ADEPT_RECORDING_PAUSABLE
      }
#endif
    }

    // Suppose the non-Adept code uses double values from n aReal
    // objects pointed to by "x" and m aReal objects pointed to by "z"
    // to produce a single double value, plus pointers to arrays of
    // forward derivatives "dy_dx" and "dy_dz".  Firstly, as above,
    // you should assign the value using simply "y = y_val;", then
    // call "y.add_derivative_dependence(x, dy_dx, n);" to specify how
    // y depends on x.  To specify also how y depends on z, call
    // "y.append_derivative_dependence(z, dy_dz, n);".
    void append_derivative_dependence(const Active<Type>* rhs,
				      const Real* multiplier,
				      int n,
				      int multiplier_stride = 1) const {
#ifdef ADEPT_RECORDING_PAUSABLE
      if (ADEPT_ACTIVE_STACK->is_recording()) {
#endif
#ifndef ADEPT_MANUAL_MEMORY_ALLOCATION
	// Check there is space in the operation stack for n entries
	ADEPT_ACTIVE_STACK->check_space(n);
#endif
	for (int i = 0; i < n; i++) {
	  Real mult = multiplier[i*multiplier_stride];
	  if (mult != 0.0) {
	    // For each non-zero multiplier, add a pseudo-operation to
	    // the operation stack
	    ADEPT_ACTIVE_STACK->push_rhs(mult,
					 rhs[i].gradient_index());
	  }
	}
	if (!(ADEPT_ACTIVE_STACK->update_lhs(gradient_index_))) {
	  throw wrong_gradient("Wrong gradient: append_derivative_dependence called on a different aReal object from the most recent add_derivative_dependence call"
			       ADEPT_EXCEPTION_LOCATION);
	}
#ifdef ADEPT_RECORDING_PAUSABLE
      }
#endif
    }
    // For only one independent variable on the rhs, these two
    // functions are convenient as they don't involve pointers
    template <class T>
    void add_derivative_dependence(T& rhs, Real multiplier) const {
      ADEPT_ACTIVE_STACK->add_derivative_dependence(gradient_index_,
						    rhs.gradient_index(),
						    multiplier);
    }
    template <class T>
    void append_derivative_dependence(T& rhs, Real multiplier) const {
      ADEPT_ACTIVE_STACK->append_derivative_dependence(gradient_index_,
						       rhs.gradient_index(),
						       multiplier);
    }
 
    // -------------------------------------------------------------------
    // 4.1. Public member functions used by other expressions
    // -------------------------------------------------------------------
    bool get_dimensions_(ExpressionSize<0>& dim) const { return true; }

    std::string expression_string_() const {
      std::stringstream s;
      s << "ActiveConstReference(" << val_ << ")";
      return s.str();
    }

    bool is_aliased_(const Type* mem1, const Type* mem2) const { 
      return &val_ >= mem1 && &val_ <= mem2; 
    }

    Type value_with_len_(const Index& j, const Index& len) const
    { return val_; }

    template <int MyArrayNum, int NArrays>
    void advance_location_(ExpressionSize<NArrays>& loc) const { } 

    template <int MyArrayNum, int NArrays>
    Type value_at_location_(const ExpressionSize<NArrays>& loc) const
    { return val_; }
    
    template <int MyArrayNum, int MyScratchNum, int NArrays, int NScratch>
    Type value_at_location_store_(const ExpressionSize<NArrays>& loc,
				internal::ScratchVector<NScratch>& scratch) const
    { return val_; }

    template <int MyArrayNum, int MyScratchNum, int NArrays, int NScratch>
    Type value_stored_(const ExpressionSize<NArrays>& loc,
		     const internal::ScratchVector<NScratch>& scratch) const
    { return val_; }

    template <int MyArrayNum, int MyScratchNum, int NArrays, int NScratch>
    void calc_gradient_(Stack& stack, 
			const ExpressionSize<NArrays>& loc,
			const internal::ScratchVector<NScratch>& scratch) const {
      stack.push_rhs(1.0, gradient_index_);
    }

    template <int MyArrayNum, int MyScratchNum, 
	      int NArrays, int NScratch, typename MyType>
    void calc_gradient_(Stack& stack, 
			const ExpressionSize<NArrays>& loc,
			const internal::ScratchVector<NScratch>& scratch,
			const MyType& multiplier) const {
      stack.push_rhs(multiplier, gradient_index_);
    }

    template <int MyArrayNum, int Rank, int NArrays>
    void set_location_(const ExpressionSize<Rank>& i, 
		       ExpressionSize<NArrays>& index) const {}


    // The Stack::independent(x) and Stack::dependent(y) functions add
    // the gradient_index of objects x and y to std::vector<uIndex>
    // objects in Stack. Since x and y may be scalars or arrays, this
    // is best done by delegating to the ActiveConstReference or Array classes.
    template <typename IndexType>
    void push_gradient_indices(std::vector<IndexType>& vec) const {
      vec.push_back(gradient_index_);
    }

    // -------------------------------------------------------------------
    // 5. Public member functions that modify the object
    // -------------------------------------------------------------------

    // Set the value 
    template <typename MyType>
    void set_value(const MyType& x) { val_ = x; }
    
    // -------------------------------------------------------------------
    // 6. Protected member functions
    // -------------------------------------------------------------------
  protected:
    
    // -------------------------------------------------------------------
    // 7. Data
    // -------------------------------------------------------------------
  private:
    const Type& val_;              // Reference to the numerical value
    Index gradient_index_;         // Index to where the corresponding
				   // gradient will be held during the
				   // adjoint calculation
  }; // End of definition of ActiveConstReference


  // ---------------------------------------------------------------------
  // Helper function for ActiveConstReference class
  // ---------------------------------------------------------------------

  template<typename Type>
  inline
  std::ostream&
  operator<<(std::ostream& os, const ActiveConstReference<Type>& v)
  {
    os << v.value();
    return os;
  }


  namespace internal {
    
    // ---------------------------------------------------------------------
    // active_const_reference
    // ---------------------------------------------------------------------

    // Return the active reference version of Type if it is active,
    // otherwise just return Type&

    template <class Type, bool IsActive> struct active_const_reference {
      typedef const Type& type;
    };
    template <class Type> struct active_const_reference<Type, true> {
      typedef ActiveConstReference<Type> type;
    };
  }



} // End namespace adept

#endif
