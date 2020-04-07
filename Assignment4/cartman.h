/*********************************************************************
 *
 * Copyright (C) 2020 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

enum track { Red, Green, Blue, Yellow, Black };
enum junction { A, B, C, D, E };

/*
 * Ask CART to sdtart crossing the critical section of TRACK starting
 * at JUNCTION.
 *
 * System will exit if:
 * 
 *   1. CART is not known to the system
 *   2. JUNCTION is already reserved
 *   3. CART is not currently at JUNCTION 
 *   4. CART is no longer active having crossed it's critical section of track
 */
void cross(unsigned int cart, enum track track, enum junction junction); 

/*
 * Reserve JUNCTION for use by CART.
 *
 * System will exit if:
 * 
 *   1. CART is not known to the system
 *   2. JUNCTION is already reserved
 *   3. CART is not currently at JUNCTION 
 *   4. CART has already crossed JUNCTION
 */
void reserve(unsigned int cart, enum junction junction);

/*
 * Release JUNCTION reserved by CART.
 *
 * System will exit if:
 * 
 *   1. CART is not known to the system
 *   2. JUNCTION is not reserved by CART
 */
void release(unsigned int cart, enum junction junction);

/*
 * Callback when CART on TRACK arrives at JUNCTION in preparation for
 * crossing a critical section of track.
 */
void arrive(unsigned int cart, enum track track, enum junction junction);

/*
 * Callback when CART on TRACK departs JUNCTION after crossing a critical
 * section of track.
 */
void depart(unsigned int cart, enum track track, enum junction junction);

/*
 * Start the CART Manager. 
 *
 * Return is optional, i.e. entering an infimnite loop is allowed, if not 
 * exactly recommended.
 */
void cartman();

