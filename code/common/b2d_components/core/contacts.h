// contacts.h

#ifndef __B2d_COMPONENTS_CONTACTS_H
#define __B2d_COMPONENTS_CONTACTS_H


namespace b2dc {

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


#endif

