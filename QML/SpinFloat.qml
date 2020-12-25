import QtQuick 2.15

import QtQuick.Controls 2.15

SpinBox {
    property int decimals: 2
    property real realValue: 0.0
    property real realFrom: 0.0
    property real realTo: 100.0
    property real realStepSize: 0.1
    property real factor: Math.pow(10, decimals)
    property real displayedValue: spinbox.value / factor
    id: spinbox
    editable: true
    stepSize: realStepSize * factor
    value: realValue * factor
    to: realTo * factor
    from: realFrom * factor
    validator: DoubleValidator {
        bottom: Math.min(spinbox.from, spinbox.to)
        top: Math.max(spinbox.from, spinbox.to)
    }

    textFromValue: function (value, locale) {
        return Number(value / 100).toLocaleString(locale, 'f', spinbox.decimals)
    }
    valueFromText: function (text, locale) {
        return Number.fromLocaleString(locale, text) * factor
    }
}
