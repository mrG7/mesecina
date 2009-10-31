template <class K> 
Dummy_3<K>::Dummy_3() : Geometry() {}

template <class K> 
Dummy_3<K>::~Dummy_3() {}

template <class K> 
Geometry* Dummy_3<K>::clone() {
	return 0;
}

template <class K> 
void Dummy_3<K>::add_weighted_points(std::list<Point4D>*) {}

template <class K> 
std::list<Point4D>* Dummy_3<K>::get_weighted_points() {
	return 0;
}

template <class K> 
void Dummy_3<K>::load_generic_file(const std::string&) {
}

template <class K> 
void Dummy_3<K>::save_generic_file(const std::string&) {
}

template <class K> 
void Dummy_3<K>::application_settings_changed(const QString&) {
}

template <class K> 
void Dummy_3<K>::apply_modification(const std::string& ) {
}

template <class K> 
void Dummy_3<K>::invalidate_cache() {
}