/***************************************************************************
 * gens-qt4: Gens Qt4 UI.                                                  *
 * ConfigStore.cpp: Configuration store.                                   *
 *                                                                         *
 * Copyright (c) 1999-2002 by Stéphane Dallongeville.                      *
 * Copyright (c) 2003-2004 by Stéphane Akhoun.                             *
 * Copyright (c) 2008-2011 by David Korth.                                 *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

#include "ConfigStore.hpp"

// C includes.
#include <stdint.h>

// LibGens includes.
#include "libgens/lg_main.hpp"
#include "libgens/macros/common.h"
#include "libgens/macros/log_msg.h"

// Qt includes.
#include <QtCore/QSettings>
#include <QtCore/QHash>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QPointer>
#include <QtCore/QVector>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

#include <QtCore/QObject>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaMethod>

// QtGui includes.
#include <QtGui/QColor>

// Default settings.
#include "ConfigDefaults.hpp"

// Key configuration.
#include "actions/GensKeyConfig.hpp"

// gqt4_app
#include "gqt4_main.hpp"
#include "GensQApplication.hpp"

namespace GensQt4
{

class ConfigStorePrivate
{
	public:
		ConfigStorePrivate(ConfigStore *q);
		~ConfigStorePrivate();
	
	private:
		ConfigStore *const q;
		Q_DISABLE_COPY(ConfigStorePrivate)
	
	public:
		/**
		 * Reset all settings to defaults.
		 */
		void reset(void);
		
		/**
		 * Check if a region code order is valid.
		 * @param regionCodeOrder Region code order to check.
		 * @return True if valid; false if invalid.
		 */
		static bool IsRegionCodeOrderValid(uint16_t regionCodeOrder);
		
		/**
		 * Validate a property.
		 * @param key Property name.
		 * @param value Property value. (May be edited for validation.)
		 * @return Property value (possibly adjusted) if validated; invalid QVariant on failure.
		 */
		static QVariant Validate(QString name, QVariant value);
		
		/**
		 * Set a property.
		 * @param key Property name.
		 * @param value Property value.
		 */
		void set(QString key, QVariant value);
		
		/**
		 * Get a property.
		 * @param key Property name.
		 * @return Property value.
		 */
		QVariant get(QString key) const;
		
		/**
		 * Get a property.
		 * Converts hexadecimal string values to unsigned int.
		 * @param key Property name.
		 * @return Property value.
		 */
		unsigned int getUInt(QString key) const;
		
		/**
		 * Get a property.
		 * Converts hexadecimal string values to signed int.
		 * @param key Property name.
		 * @return Property value.
		 */
		int getInt(QString key) const;
		
		/**
		 * Load the configuration file.
		 * @param filename Configuration filename.
		 * @return 0 on success; non-zero on error.
		 */
		int load(QString filename);
		
		/**
		 * Load the configuration file.
		 * No filename specified; use the default filename.
		 * @return 0 on success; non-zero on error.
		 */
		int load(void);
		
		/**
		 * Save the configuration file.
		 * @param filename Configuration filename.
		 * @return 0 on success; non-zero on error.
		 */
		int save(QString filename) const;
		
		/**
		 * Save the configuration file.
		 * No filename specified; use the default filename.
		 * @return 0 on success; non-zero on error.
		 */
		int save(void) const;
		
		/**
		 * Register an object for property change notification.
		 * @param property Property to watch.
		 * @param object QObject to register.
		 * @param slot Slot name.
		 */
		void registerChangeNotification(QString property, QObject *object, const char *slot);
		
		/**
		 * Unregister an object for property change notification.
		 * @param property Property to watch.
		 * @param object QObject to register.
		 * @param slot Slot name.
		 */
		void unregisterChangeNotification(QString property, QObject *object, const char *slot);
		
		/**
		 * Notify all registered objects that configuration settings have changed.
		 * Useful when starting the emulator.
		 */
		void notifyAll(void);
		
		/**
		 * Look up the method index of a SIGNAL() or SLOT() in a QObject.
		 * @param object Qt object.
		 * @param method Method name.
		 * @return Method index, or negative on error.
		 */
		static int LookupQtMethod(const QObject *object, const char *method);
		
		/**
		 * Invoke a Qt method by method index, with one QVariant parameter.
		 * @param object Qt object.
		 * @param method_idx Method index.
		 * @param param QVariant parameter.
		 */
		static void InvokeQtMethod(QObject *object, int method_idx, QVariant param);
	
		/** Internal variables. **/
		
		// Current settings.
		// TODO: Use const char* for the key instead of QString?
		QHash<QString, QVariant> settings;
		
		/**
		 * Signal mappings.
		 * Format:
		 * - Key: Property to watch.
		 * - Value: List of SignalMaps.
		 *   - SignalMap.object: Object to send signal to.
		 *   - SignalMap.method: Method name.
		 */
		struct SignalMap
		{
			QPointer<QObject> object;
			int method_idx;
		};
		QHash<QString, QVector<SignalMap>* > signalMaps;
		QMutex mtxSignalMaps;
		
		/** PathConfig **/
		PathConfig *const pathConfig;
		
		/** Recent ROMs. **/
		RecentRoms *const recentRoms;
		
		/** Key configuration. **/
		GensKeyConfig keyConfig;
};


/** ConfigStorePrivate **/


ConfigStorePrivate::ConfigStorePrivate(ConfigStore* q)
	: q(q)
	, pathConfig(new PathConfig(q))
	, recentRoms(new RecentRoms(q))
{
	// TODO: Rework this with the upcoming all-in-one IoManager.
	q->m_ctrlConfig = new CtrlConfig(q);

	// Initialize settings to defaults.
	reset();

	// Load the user's settings.
	load();
}

ConfigStorePrivate::~ConfigStorePrivate()
{
	// Save the configuration.
	// TODO: Handle non-default filenames.
	save();

	// Delete all the signal map vectors.
	qDeleteAll(signalMaps);
	signalMaps.clear();
}


/**
 * Register an object for property change notification.
 * @param property Property to watch.
 * @param object QObject to register.
 * @param method Method name.
 */
void ConfigStorePrivate::registerChangeNotification(QString property, QObject *object, const char *method)
{
	if (!object)
		return;

	// Get the vector of signal maps for this property.
	QMutexLocker mtxLocker(&mtxSignalMaps);
	QVector<SignalMap>* signalMapVector = signalMaps.value(property, nullptr);
	if (!signalMapVector) {
		// No vector found. Create one.
		signalMapVector = new QVector<SignalMap>();
		signalMaps.insert(property, signalMapVector);
	}

	// Look up the method index.
	int method_idx = LookupQtMethod(object, method);
	if (method_idx < 0)
		return;

	// Add this object and slot to the signal maps vector.
	SignalMap smap;
	smap.object = object;
	smap.method_idx = method_idx;
	signalMapVector->append(smap);
}


/**
 * Unregister an object for property change notification.
 * @param property Property to watch.
 * @param object QObject to register.
 * @param method Method name. (If nullptr, unregisters all slots for this object.)
 */
void ConfigStorePrivate::unregisterChangeNotification(QString property, QObject *object, const char *method)
{
	if (!object)
		return;

	// Get the vector of signal maps for this property.
	QMutexLocker mtxLocker(&mtxSignalMaps);
	QVector<SignalMap>* signalMapVector = signalMaps.value(property, nullptr);
	if (!signalMapVector)
		return;

	// Get the method index.
	int method_idx = -1;
	if (method != nullptr) {
		method_idx = LookupQtMethod(object, method);
		if (method_idx < 0)
			return;
	}

	// Process the signal map vector in reverse-order.
	// Reverse order makes it easier to remove deleted objects.
	// TODO: Use QLinkedList instead?
	for (int i = (signalMapVector->size() - 1); i >= 0; i--) {
		const SignalMap *smap = &signalMapVector->at(i);
		if (smap->object.isNull()) {
			signalMapVector->remove(i);
		} else if (smap->object == object) {
			// Found the object.
			if (method == nullptr || method_idx == smap->method_idx) {
				// Found a matching signal map.
				signalMapVector->remove(i);
			}
		}
	}
}


/**
 * Reset all settings to defaults.
 */
void ConfigStorePrivate::reset(void)
{
	// Initialize settings with DefaultSettings.
	settings.clear();
	for (const ConfigDefaults::DefaultSetting *def = &ConfigDefaults::DefaultSettings[0];
	     def->key != nullptr; def++)
	{
		settings.insert(QLatin1String(def->key),
				(def->value ? QLatin1String(def->value) : QString()));
	}
}


/**
 * Check if a region code order is valid.
 * @param regionCodeOrder Region code order to check.
 * @return True if valid; false if invalid.
 */
bool ConfigStorePrivate::IsRegionCodeOrderValid(uint16_t regionCodeOrder)
{
	static const uint16_t RegionCodeOrder_tbl[24] =
	{
		0x4812, 0x4821, 0x4182, 0x4128, 0x4281, 0x4218, 
		0x8412, 0x8421, 0x8124, 0x8142, 0x8241, 0x8214,
		0x1482, 0x1428, 0x1824, 0x1842, 0x1248, 0x1284,
		0x2481, 0x2418,	0x2814, 0x2841, 0x2148, 0x2184
	};

	for (size_t i = 0; i < ARRAY_SIZE(RegionCodeOrder_tbl); i++) {
		if (regionCodeOrder == RegionCodeOrder_tbl[i])
			return true;
	}

	// Region code order is not valid.
	return false;
}


/**
 * Validate a property.
 * @param key Property name.
 * @param value Property value. (May be edited for validation.)
 * @return Property value (possibly adjusted) if validated; invalid QVariant on failure.
 */
QVariant ConfigStorePrivate::Validate(QString name, QVariant value)
{
	// Get the DefaultSetting entry for this property.
	// TODO: Lock the hash?
	const ConfigDefaults::DefaultSetting *def = ConfigDefaults::Instance()->get(name);
	if (!def)
		return -1;

	switch (def->validation) {
		case ConfigDefaults::DefaultSetting::VT_NONE:
		default:
			// No validation required.
			return value;

		case ConfigDefaults::DefaultSetting::VT_BOOL:
			if (!value.canConvert(QVariant::Bool))
				return QVariant();
			return QVariant(value.toBool());

		case ConfigDefaults::DefaultSetting::VT_COLOR:
		{
			QColor color = value.value<QColor>();
			if (!color.isValid())
				return QVariant();
			return QVariant(color.name());
		}

		case ConfigDefaults::DefaultSetting::VT_RANGE:
		{
			if (!value.canConvert(QVariant::Int))
				return QVariant();
			int val = value.toString().toInt(nullptr, 0);
			if (val < def->range_min || val > def->range_max)
				return QVariant();
			return QVariant(val);
		}

		case ConfigDefaults::DefaultSetting::VT_REGIONCODEORDER:
		{
			if (!value.canConvert(QVariant::UInt))
				return QVariant();
			uint16_t rc_order = (uint16_t)value.toString().toUInt(nullptr, 0);
			if (!IsRegionCodeOrderValid(rc_order))
				return QVariant();
			return QVariant(rc_order);
		}
	}

	// Should not get here...
	return QVariant();
}


/**
 * Set a property.
 * @param key Property name.
 * @param value Property value.
 */
void ConfigStorePrivate::set(QString key, QVariant value)
{
#ifndef NDEBUG
	// Make sure this property exists.
	if (!settings.contains(key)) {
		// Property does not exist. Print a warning.
		// TODO: Make this an error, since it won't be saved?
		LOG_MSG(gens, LOG_MSG_LEVEL_WARNING,
			"ConfigStorePrivate: Property '%s' has no default value. FIX THIS!",
			key.toUtf8().constData());
	}
#endif

	// Get the default value.
	const ConfigDefaults::DefaultSetting *def = ConfigDefaults::Instance()->get(key);
	if (!def)
		return;

	if (!(def->flags & ConfigDefaults::DefaultSetting::DEF_ALLOW_SAME_VALUE)) {
		// Check if the new value is the same as the old value.
		QVariant oldValue = settings.value(key);
		if (value == oldValue)
			return;
	}

	// Verify that this value passes validation.
	QVariant newValue = Validate(key, value);
	if (!newValue.isValid())
		return;

	// Set the new value.
	settings.insert(key, newValue);

	// Invoke methods for registered objects.
	QMutexLocker mtxLocker(&mtxSignalMaps);
	QVector<SignalMap> *signalMapVector = signalMaps.value(key, nullptr);
	if (!signalMapVector)
		return;

	// Process the signal map vector in reverse-order.
	// Reverse order makes it easier to remove deleted objects.
	// TODO: Use QLinkedList instead?
	for (int i = (signalMapVector->size() - 1); i >= 0; i--) {
		const SignalMap *smap = &signalMapVector->at(i);
		if (smap->object.isNull()) {
			signalMapVector->remove(i);
		} else {
			// Invoke this method.
			InvokeQtMethod(smap->object, smap->method_idx, newValue);
		}
	}
}


/**
 * Get a property.
 * @param key Property name.
 * @return Property value.
 */
QVariant ConfigStorePrivate::get(QString key) const
{
#ifndef NDEBUG
	// Make sure this property exists.
	if (!settings.contains(key))
	{
		// Property does not exist. Print a warning.
		// TODO: Make this an error, since it won't be saved?
		LOG_MSG(gens, LOG_MSG_LEVEL_WARNING,
			"ConfigStorePrivate: Property '%s' has no default value. FIX THIS!",
			key.toUtf8().constData());
	}
#endif

	return settings.value(key);
}

/**
 * Get a property.
 * Converts hexadecimal string values to unsigned int.
 * @param key Property name.
 * @return Property value.
 */
unsigned int ConfigStorePrivate::getUInt(QString key) const
	{ return get(key).toString().toUInt(nullptr, 0); }

/**
 * Get a property.
 * Converts hexadecimal string values to signed int.
 * @param key Property name.
 * @return Property value.
 */
int ConfigStorePrivate::getInt(QString key) const
	{ return get(key).toString().toInt(nullptr, 0); }


/**
 * Load the configuration file.
 * @param filename Configuration filename.
 * @return 0 on success; non-zero on error.
 */
int ConfigStorePrivate::load(QString filename)
{
	QSettings qSettings(filename, QSettings::IniFormat);

	// NOTE: Only known settings will be loaded.
	settings.clear();
	// TODO: Add function to get sizeof(DefaultSettings) from ConfigDefaults.
	settings.reserve(32);

	// Load known settings from the configuration file.
	for (const ConfigDefaults::DefaultSetting *def = &ConfigDefaults::DefaultSettings[0];
	     def->key != nullptr; def++)
	{
		const QString key = QLatin1String(def->key);
		QVariant value = qSettings.value(key, QLatin1String(def->value));

		// Validate this value.
		value = Validate(key, value);
		if (!value.isValid()) {
			// Validation failed. Use the default value.
			value = QVariant(QLatin1String(def->value));
		}

		settings.insert(key, value);
	}

	// Load the PathConfig settings.
	qSettings.beginGroup(QLatin1String("Directories"));
	pathConfig->load(&qSettings);
	qSettings.endGroup();

	// Load the Recent ROMs settings.
	qSettings.beginGroup(QLatin1String("Recent_ROMs"));
	recentRoms->load(&qSettings);
	qSettings.endGroup();

	// Load the key configuration.
	qSettings.beginGroup(QLatin1String("Shortcut_Keys"));
	keyConfig.load(&qSettings);
	qSettings.endGroup();

	// Load the controller configuration.
	// TODO: Rework this with the upcoming all-in-one IoManager.
	qSettings.beginGroup(QLatin1String("Controllers"));
	q->m_ctrlConfig->load(&qSettings);
	qSettings.endGroup();

	// Finished loading settings.
	// NOTE: Caller must call emitAll() for settings to take effect.
	return 0;
}

/**
 * Load the configuration file.
 * No filename specified; use the default filename.
 * @return 0 on success; non-zero on error.
 */
int ConfigStorePrivate::load(void)
{
	const QString cfgFilename = (pathConfig->configPath() +
					QLatin1String(ConfigDefaults::DefaultConfigFilename));
	return load(cfgFilename);
}


/**
 * Save the configuration file.
 * @param filename Configuration filename.
 * @return 0 on success; non-zero on error.
 */
int ConfigStorePrivate::save(QString filename) const
{
	QSettings qSettings(filename, QSettings::IniFormat);

	/** Application information. **/

	// Stored in the "General" section.
	// TODO: Move "General" settings to another section?
	// ("General" is always moved to the top of the file.)
	qSettings.setValue(QLatin1String("_Application"), gqt4_app->applicationName());
	qSettings.setValue(QLatin1String("_Version"), gqt4_app->applicationVersion());

	if (LibGens::version_desc) {
		qSettings.setValue(QLatin1String("_VersionExt"),
					QString::fromUtf8(LibGens::version_desc));
	} else {
		qSettings.remove(QLatin1String("_VersionExt"));
	}

	if (LibGens::version_vcs) {
		qSettings.setValue(QLatin1String("_VersionVcs"),
					QString::fromUtf8(LibGens::version_vcs));
	} else {
		qSettings.remove(QLatin1String("_VersionVcs"));
	}

	// NOTE: Only known settings will be saved.
	
	// Save known settings to the configuration file.
	for (const ConfigDefaults::DefaultSetting *def = &ConfigDefaults::DefaultSettings[0];
	     def->key != nullptr; def++)
	{
		if (def->flags & ConfigDefaults::DefaultSetting::DEF_NO_SAVE)
			continue;

		const QString key = QLatin1String(def->key);
		QVariant value = settings.value(key, QLatin1String(def->value));
		if (def->hex_digits > 0) {
			// Convert to hexadecimal.
			unsigned int uint_val = value.toString().toUInt(nullptr, 0);
			QString str = QLatin1String("0x") +
					QString::number(uint_val, 16).toUpper().rightJustified(4, QChar(L'0'));
			value = str;
		}

		qSettings.setValue(key, value);
	}

	// Save the PathConfig settings.
	qSettings.beginGroup(QLatin1String("Directories"));
	pathConfig->save(&qSettings);
	qSettings.endGroup();

	// Save the Recent ROMs settings.
	qSettings.beginGroup(QLatin1String("Recent_ROMs"));
	recentRoms->save(&qSettings);
	qSettings.endGroup();

	// Save the key configuration.
	qSettings.beginGroup(QLatin1String("Shortcut_Keys"));
	keyConfig.save(&qSettings);
	qSettings.endGroup();

	// Save the controller configuration.
	// TODO: Rework this with the upcoming all-in-one IoManager.
	qSettings.beginGroup(QLatin1String("Controllers"));
	q->m_ctrlConfig->save(&qSettings);
	qSettings.endGroup();

	return 0;
}

/**
 * Save the configuration file.
 * No filename specified; use the default filename.
 * @return 0 on success; non-zero on error.
 */
int ConfigStorePrivate::save(void) const
{
	const QString cfgFilename = (pathConfig->configPath() +
					QLatin1String(ConfigDefaults::DefaultConfigFilename));
	return save(cfgFilename);
}


/**
 * Notify all registered objects that configuration settings have changed.
 * Useful when starting the emulator.
 */
void ConfigStorePrivate::notifyAll(void)
{
	// Invoke methods for registered objects.
	QMutexLocker mtxLocker(&mtxSignalMaps);

	foreach (QString property, signalMaps.keys()) {
		QVector<SignalMap> *signalMapVector = signalMaps.value(property);
		if (signalMapVector->isEmpty())
			continue;

		// Get the property value.
		const QVariant value = settings.value(property);

		// Process the signal map vector in reverse-order.
		// Reverse order makes it easier to remove deleted objects.
		// TODO: Use QLinkedList instead?
		for (int i = (signalMapVector->size() - 1); i >= 0; i--)
		{
			const SignalMap *smap = &signalMapVector->at(i);
			if (smap->object.isNull()) {
				signalMapVector->remove(i);
			} else {
				// Invoke this method.
				InvokeQtMethod(smap->object, smap->method_idx, value);
			}
		}
	}
}


/**
 * Look up the method index of a SIGNAL() or SLOT() in a QObject.
 * @param object Qt object.
 * @param method Method name.
 * @return Method index, or negative on error.
 */
int ConfigStorePrivate::LookupQtMethod(const QObject *object, const char *method)
{
	// Based on QMetaObject::invokeMethod().
	
	// NOTE: The first character of method indicates whether it's a signal or slot.
	// We don't need this information, so we use method+1.
	method++;
	
	int idx = object->metaObject()->indexOfMethod(method);
	if (idx < 0)
	{
		QByteArray norm = QMetaObject::normalizedSignature(method);
		idx = object->metaObject()->indexOfMethod(norm.constData());
	}
	
	if (idx < 0 || idx >= object->metaObject()->methodCount())
	{
		// TODO: Do verification in registerChangeNotification()?
		LOG_MSG(gens, LOG_MSG_LEVEL_WARNING,
			"No such method %s::%s",
			object->metaObject()->className(), method);
		return -1;
	}
	
	// Method index found.
	return idx;
}

/**
 * Invoke a Qt method by method index, with one QVariant parameter.
 * @param object Qt object.
 * @param method_idx Method index.
 * @param param QVariant parameter.
 */
void ConfigStorePrivate::InvokeQtMethod(QObject *object, int method_idx, QVariant param)
{
	// Based on QMetaObject::invokeMethod().
	QMetaMethod metaMethod = object->metaObject()->method(method_idx);
	metaMethod.invoke(object, Qt::AutoConnection,
		      QGenericReturnArgument(), Q_ARG(QVariant, param));
}


/** ConfigStore **/


ConfigStore::ConfigStore(QObject *parent)
	: QObject(parent)
	, d(new ConfigStorePrivate(this))
{ }

ConfigStore::~ConfigStore()
	{ delete d; }


/**
 * Reset all settings to defaults.
 */
void ConfigStore::reset(void)
	{ d->reset(); }


/**
 * Set a property.
 * @param key Property name.
 * @param value Property value.
 */
void ConfigStore::set(QString key, QVariant value)
	{ d->set(key, value); }


/**
 * Get a property.
 * @param key Property name.
 * @return Property value.
 */
QVariant ConfigStore::get(QString key) const
	{ return d->get(key); }

/**
 * Get a property.
 * Converts hexadecimal string values to unsigned int.
 * @param key Property name.
 * @return Property value.
 */
unsigned int ConfigStore::getUInt(QString key) const
	{ return d->getUInt(key); }

/**
 * Get a property.
 * Converts hexadecimal string values to signed int.
 * @param key Property name.
 * @return Property value.
 */
int ConfigStore::getInt(QString key) const
	{ return d->getInt(key); }


/**
 * Load the configuration file.
 * @param filename Configuration filename.
 * @return 0 on success; non-zero on error.
 */
int ConfigStore::load(QString filename)
	{ return d->load(filename); }

/**
 * Load the configuration file.
 * No filename specified; use the default filename.
 * @return 0 on success; non-zero on error.
 */
int ConfigStore::load(void)
	{ return d->load(); }


/**
 * Save the configuration file.
 * @param filename Filename.
 * @return 0 on success; non-zero on error.
 */
int ConfigStore::save(QString filename) const
	{ return d->save(filename); }

/**
 * Save the configuration file.
 * No filename specified; use the default filename.
 * @return 0 on success; non-zero on error.
 */
int ConfigStore::save(void) const
	{ return d->save(); }


/**
 * Register an object for property change notification.
 * @param property Property to watch.
 * @param object QObject to register.
 * @param method Method name.
 */
void ConfigStore::registerChangeNotification(QString property, QObject *object, const char *method)
	{ d->registerChangeNotification(property, object, method); }

/**
 * Unregister an object for property change notification.
 * @param property Property to watch.
 * @param object QObject to register.
 * @param method Method name.
 */
void ConfigStore::unregisterChangeNotification(QString property, QObject *object, const char *method)
	{ d->unregisterChangeNotification(property, object, method); }

/**
 * Notify all registered objects that configuration settings have changed.
 * Useful when starting the emulator.
 */
void ConfigStore::notifyAll(void)
	{ d->notifyAll(); }

/**
 * Get the main configuration path. (GCPATH_CONFIG)
 * @return Main configuration path.
 */
QString ConfigStore::configPath(void) const
	{ return d->pathConfig->configPath(); }

/**
 * Get the specified configuration path.
 * @param path Configuration path to get. (Invalid paths act like GCPATH_CONFIG.)
 * @return Configuration path.
 */
QString ConfigStore::configPath(PathConfig::ConfigPath path) const
	{ return d->pathConfig->configPath(path); }

/**
 * Get a const pointer to the PathConfig object.
 * @return Const pointer to the PathConfig object.
 */
const PathConfig *ConfigStore::pathConfigObject(void) const
	{ return d->pathConfig; }


/** Recent ROMs. **/

/**
 * Update the Recent ROMs list.
 * @param filename ROM filename.
 * @param z_filename Filename of ROM within archive.
 * @param sysId System ID.
 */
void ConfigStore::recentRomsUpdate(QString filename, QString z_filename,
					LibGens::Rom::MDP_SYSTEM_ID sysId)
{
	d->recentRoms->update(filename, z_filename, sysId);
}

/**
 * Get a const pointer to the Recent ROMs object.
 * @return Const pointer to the Recent ROMs object.
 */
const RecentRoms *ConfigStore::recentRomsObject(void) const
	{ return d->recentRoms; }

/**
 * Get a Recent ROMs entry.
 * @param id Recent ROM ID.
 */
RecentRom_t ConfigStore::recentRomsEntry(int id) const
	{ return d->recentRoms->getRom(id); }


/** Key configuration. **/

/**
 * Get the action associated with a GensKey_t.
 * @param key GensKey_t.
 * @return Action ID.
 */
int ConfigStore::keyToAction(GensKey_t key) const
	{ return d->keyConfig.keyToAction(key); }

/**
 * Get the GensKey_t associated with an action.
 * @param actoin Action ID.
 * @return GensKey_t.
 */
GensKey_t ConfigStore::actionToKey(int action) const
	{ return d->keyConfig.actionToKey(action); }

}
