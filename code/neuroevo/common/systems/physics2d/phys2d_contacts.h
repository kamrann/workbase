// phys2d_contacts.h

#ifndef __NE_PHYS2D_CONTACTS_H
#define __NE_PHYS2D_CONTACTS_H


namespace sys {
	namespace phys2d {

		enum class ContactType {
			Begin,
			End,
		};

		struct collision_filter
		{
			int group_index;
			// todo: masks

			collision_filter():
				group_index{ 0 }
			{}
		};

	}
}


#endif

