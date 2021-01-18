
Vue.component('component-properties', {
  props: [ "name", "value" ],
  methods: {
  },
  template: `
    <div>
      <div class="component-key">{{ name }}</div>
      <property-value :value="value"></property-value>
    </div>
    `
});
  