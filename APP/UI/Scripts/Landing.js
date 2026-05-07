/*
    Logging toggle element
*/
const LoggingToggle =
    document.getElementById(
        'LoggingToggle'
    );


/*
    Toggle logging
*/
LoggingToggle.addEventListener(

    'change',

    async () => {

        /*
            Toggle state
        */
        const Enabled =
            LoggingToggle.checked;


        /*
            Logging ON
        */
        if (Enabled) {

            /*
                LOG_TEXT = 0
                LOG_UDS  = 1
                LOG_CAN  = 2
            */
            await window.Sutrika
                .Log
                .State(true, 1);

            console.log(
                'Logging Enabled'
            );
        }

        /*
            Logging OFF
        */
        else {

            await window.Sutrika
                .Log
                .State(false);

            console.log(
                'Logging Disabled'
            );
        }
    }
);