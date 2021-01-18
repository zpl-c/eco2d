
Vue.component('property-value', {
  props: [ "value" ],
  methods: {
    css() {
      var result = "property-value";
      if (this.type.length) {
        result += " property-value-" + this.type;
      }
      return result;
    }
  },
  computed: {
    type: function() {
      if (Array.isArray(this.value)) {
        return 'array';
      } else if (typeof this.value === 'object') {
        return 'object';
      } else if (typeof this.value === 'number') {
        return 'number';
      } else if (typeof this.value === 'boolean') {
        return 'boolean';
      } else if (typeof this.value === 'string') {
        return 'string';
      } else {
        return '';
      }
    }
  },
  template: `
    <div :class="css()">
      <div v-if="type === 'object'">

      </div>
      <div v-else-if="type === 'array'">

      </div>
      <div v-else>
        <input :value="value" spellcheck="false"></input>
      </div>
    </div>
  `
});
