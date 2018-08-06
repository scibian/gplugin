/*
 * Copyright (C) 2013 Ankit Vani <a@nevitus.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DYNAMIC_TEST_H
#define DYNAMIC_TEST_H

#define DYNAMIC_TYPE_TEST            (dynamic_test_get_type())
#define DYNAMIC_TEST(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), DYNAMIC_TYPE_TEST, DynamicTest))
#define DYNAMIC_TEST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), DYNAMIC_TYPE_TEST, DynamicTestClass))
#define DYNAMIC_IS_TEST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), DYNAMIC_TYPE_TEST))
#define DYNAMIC_IS_TEST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), DYNAMIC_TYPE_TEST))
#define DYNAMIC_TEST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), DYNAMIC_TYPE_TEST, DynamicTestClass))

typedef struct _DynamicTest          DynamicTest;
typedef struct _DynamicTestClass     DynamicTestClass;

#include <glib.h>
#include <glib-object.h>

struct _DynamicTest {
	GObject gparent;
};

struct _DynamicTestClass {
	GObjectClass gparent;
};

GType dynamic_test_get_type(void);

#endif /* DYNAMIC_TEST_H */
