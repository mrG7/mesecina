template <class K> 
Moebius_3<K>::Moebius_3() : Geometry() {}

template <class K> 
Moebius_3<K>::~Moebius_3() {}

template <class K> 
Geometry* Moebius_3<K>::clone() {
	return 0;
}

template <class K> 
void Moebius_3<K>::add_weighted_points(std::list<Point4D>*) {
	std::cout << "getting weighted points at moebius_3" << std::endl;
}

template <class K> 
std::list<Point4D>* Moebius_3<K>::get_weighted_points() {
	return 0;
}

template <class K> 
void Moebius_3<K>::load_generic_file(const std::string&) {
}

template <class K> 
void Moebius_3<K>::save_generic_file(const std::string&) {
}

template <class K> 
void Moebius_3<K>::application_settings_changed(const QString&) {
}

template <class K> 
void Moebius_3<K>::apply_modification(const std::string& ) {
}

template <class K> 
void Moebius_3<K>::invalidate_cache() {
}